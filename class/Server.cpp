/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:16:39 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/04 17:11:29 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::PrintFunctionError(std::string file, int line, std::string error, int err){
	if (_server)
		freeaddrinfo(_server);
	_server = NULL;
	if (fd != -1)
		close(fd);
	fd = -1;
	if (epollfd != -1)
		close(epollfd);
	epollfd = -1;
	print_error(file, line, error, err);
	throw FunctionError();
}

Server::Server(char *port, char *pass) : fd(-1), epollfd(-1), addr(sockaddr_in()), _hints(addrinfo()), _server(NULL), ev(epoll_event()) {

	(void)pass; //

	_hints.ai_family = AF_INET;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, port, &_hints, &_server) != 0)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	fd = socket(_server->ai_family, _server->ai_socktype, _server->ai_protocol);
	if (fd == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	int optval = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	if (bind(fd, _server->ai_addr, _server->ai_addrlen) == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	if (listen(fd, MAX_EVENTS) == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	freeaddrinfo(_server);
	_server = NULL;

	epollfd = epoll_create1(0);
	if (epollfd == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	ev.events = EPOLLIN;
	ev.data.fd = fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	struct sigaction act, oldact;
	act.sa_handler = sigint_handler;
	if (sigemptyset(&act.sa_mask) == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	act.sa_flags = 0;
	if (sigaction(SIGINT, NULL, &oldact) == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	if (sigaction(SIGINT, &act, NULL) == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);
}

int	Server::NewClient(void) {
	sockaddr_in	peer_addr = {};
	socklen_t	peer_addr_size = sizeof(peer_addr);

	int	tmpfd = accept(fd, (sockaddr *) &peer_addr, &peer_addr_size);
	if (tmpfd == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	clients.insert(std::pair<int, Client*>(tmpfd, new Client(peer_addr, peer_addr_size)));

	if (fcntl(tmpfd, F_SETFL, O_NONBLOCK) == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = tmpfd;

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, tmpfd, &ev) == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	return tmpfd;
}

bool	Server::GetUserInfo(int user_fd, std::string message){
	std::istringstream iss(message);
	std::string	line;
	std::string	password;
	std::string	nickname;
	std::string	username;

	if (std::getline(iss, line)){
		std::istringstream	linestream(line);
		std::string			word;
		std::string			prev;

		while (linestream >> word){
			if (prev == "PASS"){
				if (!word.empty() && word != "NICK" && word !="USER")
					password = word;
				else
					return std::cout << "FAIL PASS" << std::endl, false;
			}
			else if (prev == "NICK"){
				if (!word.empty() && word != "PASS" && word !="USER")
					nickname = word;
				else
					return std::cout << "FAIL NICK" << std::endl, false;
			}
			else if (prev == "USER"){
				if (!word.empty() && word != "PASS" && word !="NICK")
					username = word;
				else
					return std::cout << "FAIL USER" << std::endl, false;
			}
			prev = word;
		}
	}
	clients[user_fd]->setNickname(nickname); // verification necessaire pour validité des nick pass et user
	clients[user_fd]->setPassword(password);
	clients[user_fd]->setUsername(username);
	return true;
}

void	Server::ExistingClient(int user_fd) {
	int	user_data_fd = events[user_fd].data.fd;

	std::cout << "Existing Client fd " << user_fd << std::endl;
	while (_buffer.find("\r\n") == std::string::npos){
		char	buffer[512];
		std::memset(&buffer, 0, 512);

		int rd = recv(events[user_fd].data.fd, buffer, 512, 0);
		if (rd == -1 && errno != EAGAIN)
			return;
			// PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);
		else if (rd == 0) {
			close(user_data_fd);
			delete clients[user_data_fd];
			clients.erase(user_data_fd);
			std::cout << "client with fd " << user_fd << " has been erased" << std::endl;
		}
		std::string tmp(buffer);
		_buffer = _buffer + tmp;
		std::cout << "While \\r\\n not found : `" << _buffer << "`" << std::endl;
	}
	std::cout << "Buffer = `" << _buffer << "`" << std::endl;
	std::string message(_buffer);
	_buffer.clear();
	if (!clients[user_data_fd]->getWelcome()){
		if (!GetUserInfo(user_data_fd, message)){
			std::string error = "Wrong format, can't connect";
			send(user_data_fd, error.c_str(), error.size(), 0);
			// close(user_data_fd); // est-ce qu'on le close ?
			// clients.erase(user_data_fd); //est-ce qu'on le supprime ?
		}
		std::string welcome = 	"001 " + clients[user_data_fd]->getUsername() + " " + clients[user_data_fd]->getNickname() + " "\
								": Welcome to our Server. A echapus & mjourno network !\r\n";
		send(user_data_fd, welcome.c_str(), welcome.size(), 0);
		clients[user_data_fd]->Welcomed();
		return ;
	}
	if (!message.empty()){
		std::cout << "Suite des opérations plus tard pour l'instant voilà le reste du message : " << std::endl;
		std::cout << message;
	}
	message.clear();
}


void	Server::Launch() {
	while (1) {
		number_fds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (number_fds == -1)
			PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

		for (int i = 0; i < number_fds; i++) {
			//Premiere connexion
			if (events[i].data.fd == fd) {
				int tmpfd = NewClient();

				//tests
				std::cout << clients[tmpfd]->getWelcome() << " est le status de welcome pour le client fd " << tmpfd << std::endl;
				std::cout << "connected with fd " << tmpfd << std::endl;
				char *ip;
				ip = inet_ntoa(clients[tmpfd]->getAddr().sin_addr); //verif return value
				std::cout << "ip: " << ip << " port: " << ntohs(clients[tmpfd]->getAddr().sin_port) << std::endl;

			}//deja connecté
			else {
				ExistingClient(i);
			}
		}
	}
}

Server::~Server() {
	if (fd != -1)
		close(fd);
	if (epollfd != -1)
		close(epollfd);

	std::map<int, Client*>::iterator	it;
	for (it = clients.begin(); it != clients.end(); ++it) {
		close(it->first);
		delete it->second;
	}
	clients.clear();
}