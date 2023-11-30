/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: echapus <echapus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:16:39 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/30 19:19:55 by echapus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::PrintFunctionError(std::string file, int line, std::string error, int err){
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

Server::Server(char *port, std::string pass) : fd(-1), epollfd(-1), _hints(addrinfo()), _server(NULL), ev(epoll_event()) {

	if (pass.empty() || pass.find(' ') != std::string::npos)
		PrintFunctionError(__FILE__, __LINE__, "Invalid Password", 0);
	_pass = pass;

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

int		Server::NewClient(void) {
	sockaddr_in	peer_addr = {};
	socklen_t	peer_addr_size = sizeof(peer_addr);

	int	tmpfd = accept(fd, (sockaddr *) &peer_addr, &peer_addr_size);
	if (tmpfd == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	clients.insert(std::pair<int, Client*>(tmpfd, new Client(peer_addr, peer_addr_size)));
	bufferMap.insert(std::pair<int, std::string>(tmpfd, ""));

	if (fcntl(tmpfd, F_SETFL, O_NONBLOCK) == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = tmpfd;

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, tmpfd, &ev) == -1)
		PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

	return tmpfd;
}

void	Server::QuitAndDelete(int user_fd){
	Client *client = clients[user_fd];

	std::vector<std::string>	channel_names;

	std::string reply = ":" + client->getNickname() + "! QUIT :Quit: Abrupt quit" + CRLF;
	for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
		if (it->second->isOnChannel(client->getNickname()))
			channel_names.push_back(it->second->getName());

	for (std::vector<std::string>::iterator chan_it = channel_names.begin(); chan_it != channel_names.end(); ++chan_it){
		std::map<std::string, Channel *>::iterator it = channels.find(*chan_it);
		if (it != channels.end()){
			it->second->removeMember(*client);
			it->second->broadcastChannel(reply, client->getfd());
			if (!it->second->getCurrentUser()){
				printlog("channel " + it->second->getName() + " has been deleted", LOGS);
				delete it->second;
				channels.erase(it);
			}
		}
	}

	DeleteClient(user_fd);
}

void	Server::DeleteClient(int user_fd){
	delete clients[user_fd];
	clients.erase(user_fd);
	bufferMap.erase(user_fd);
	close(user_fd);
	std::cout << "\033[0;91m<Server LOGS>\033[0;39m client with fd " << user_fd << " has been erased" << std::endl;
}

bool	Server::FillBuffer(int user_fd){
	char	buffer[512];
	int		data_fd = events[user_fd].data.fd;
	std::memset(&buffer, 0, 512);

	printlog("Entering Fillbuffer func", LOGS);

	int rd = recv(data_fd, buffer, 512, MSG_DONTWAIT);
	std::cout << rd << " " << (errno != EAGAIN) << std::endl;
	if (rd == -1 && errno != EAGAIN)
		return false;
	else if (rd == 0)
		return QuitAndDelete(data_fd), false;

	if (rd == 512)
		buffer[rd - 1] = 0; // pour eviter conditionnal jump dans le prochain if
	
	std::string tmp(buffer);
	bufferMap[data_fd] += tmp;

	if (bufferMap[data_fd].size() > 512 || (rd == 512 && tmp.find(CRLF) == std::string::npos))
		return printlog("Server don't support message more than 512 characters.", data_fd), bufferMap[data_fd] = "", false;

	if (tmp.find(CRLF) == std::string::npos)
		return printlog("CRLF not found", LOGS), false;
	
	
	return true;
}

void	Server::ExistingClient(int user_fd) {
	int	user_data_fd = events[user_fd].data.fd;

	if (!FillBuffer(user_fd))
		return ;
	printlog(bufferMap[user_data_fd], RECV);
	std::string message(bufferMap[user_data_fd]);
	bufferMap[user_data_fd] = "";

	Client *client = clients[user_data_fd];
	client->setfd(user_data_fd);

	if (!message.empty())
		parse_command(message, client);
	message.clear();
}

Client	*Server::getClientByNickname(std::string nick){
	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it){
		if (it->second->getNickname() == nick)
			return (it->second);
	}
	return NULL;
}

void	Server::Launch() {
	while (1) {
		number_fds = epoll_wait(epollfd, events, MAX_EVENTS, -1); // maxevent à mettre à 1 quand 2 events simultanés pour evité segfault
		if (number_fds == -1)
			PrintFunctionError(__FILE__, __LINE__, std::strerror(errno), errno);

		for (int i = 0; i < number_fds; i++) {
			//Premiere connexion
			if (events[i].data.fd == fd) {
				int tmpfd = NewClient();

				//tests
				std::cout << "connected with fd " << tmpfd << std::endl;
				char *ip;
				ip = inet_ntoa(clients[tmpfd]->getAddr().sin_addr); //verif return value
				std::cout << "ip: " << ip << " port: " << ntohs(clients[tmpfd]->getAddr().sin_port) << std::endl;
			}//deja connecté
			else
				ExistingClient(i);
		}
	}
}

Server::~Server() {
	if (fd != -1)
		close(fd);
	if (epollfd != -1)
		close(epollfd);

	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it){
		close(it->first);
		delete it->second;
	}
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
		delete it->second;
	clients.clear();
}
