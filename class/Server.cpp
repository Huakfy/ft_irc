/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:16:39 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/04 11:58:18 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//getaddrinfo au lieu de sockaddr_in -> remplir addr avec
//map pour clients

Server::Server(char *port, char *pass) : fd(-1), epollfd(-1), addr(sockaddr_in()), _hints(addrinfo()), ev(epoll_event()) {

	(void)pass;

	_hints.ai_family = AF_INET;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, port, &_hints, &_server) < 0){
		print_error(__FILE__, __LINE__, std::strerror(errno), errno); //error
	}

	// fd = socket(AF_INET, SOCK_STREAM, 0); //socket to listen on port
	fd = socket(_server->ai_family, _server->ai_socktype, _server->ai_protocol);
	if (fd == -1)
		print_error(__FILE__, __LINE__, std::strerror(errno), errno); //error

	int optval = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		print_error(__FILE__, __LINE__, std::strerror(errno), errno); //error

	// socklen_t	len = sizeof(addr);
	// std::memset(&addr, 0, len);

	// addr.sin_family = AF_INET;
	// addr.sin_addr.s_addr = INADDR_ANY;

	// This ip address will change according to the machine
	// addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //changer ip

	// std::stringstream	ss(port);
	// int	i;
	// ss >> i;
	// addr.sin_port = htons(i);

	// bind(fd, (sockaddr *) &addr, len);
	//if (bind(fd, (struct sockaddr *) &addr, len) == -1)
	//	return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

	if (bind(fd, _server->ai_addr, _server->ai_addrlen) == -1)
		print_error(__FILE__, __LINE__, std::strerror(errno), errno); //error


	if (listen(fd, MAX_EVENTS) == -1)
		print_error(__FILE__, __LINE__, std::strerror(errno), errno); //error
	freeaddrinfo(_server);
	//if (listen(fd, 10) == -1) //changer 10
	//	return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

	epollfd = epoll_create1(0);
	//if (epollfd == -1)
	//	return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

	ev.events = EPOLLIN;
	ev.data.fd = fd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
	//if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
	//	return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

	struct sigaction act, oldact;
	act.sa_handler = sigint_handler;
	sigemptyset(&act.sa_mask); // if == -1 errno
	act.sa_flags = 0;
	sigaction(SIGINT, NULL, &oldact); // if == -1 errno
	sigaction(SIGINT, &act, NULL); // if == -1 errno
}


void	Server::Launch() {
	while (1) {
		number_fds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		//if (number_fds == -1)
		//	return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

		for (int i = 0; i < number_fds; i++) {
			//Premiere connexion
			if (events[i].data.fd == fd) {
				//cfd = accept(fd, (struct sockaddr *) &peer_addr, &peer_addr_size);

				sockaddr_in	peer_addr = {};
				socklen_t	peer_addr_size = sizeof(peer_addr);
				int	tmpfd = accept(fd, (sockaddr *) &peer_addr, &peer_addr_size);
				//if (cfd == -1)
				//	return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

				clients.insert(std::pair<int, Client>(fd, Client(peer_addr, peer_addr_size)));

				fcntl(tmpfd, F_SETFL, O_NONBLOCK);
				//if (fcntl(cfd, F_SETFL, O_NONBLOCK) == -1) //setnonblocking
				//	return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = tmpfd;

				epoll_ctl(epollfd, EPOLL_CTL_ADD, tmpfd, &ev);
				//if (epoll_ctl(epollfd, EPOLL_CTL_ADD, cfd, &ev) == -1)
				//	return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

				std::cout << "connected" << std::endl;

				char *ip;
				ip = inet_ntoa(clients.end()->second.getAddr().sin_addr);
				std::cout << "ip: " << ip << " port: " << ntohs(clients.end()->second.getAddr().sin_port) << std::endl;

				char buffer1[256], buffer2[256];
				std::memset(&buffer2, 0, 256);
				recv(tmpfd, buffer2, 256, 0);
				//if (recv(cfd, buffer2, 256, 0) == -1) // verif eagain
				//	return print_error(__FILE__, __LINE__, std::strerror(errno), errno);
				std::cout << "Client : " << buffer2 << std::endl;

				std::memset(&buffer1, 0, 256);
				strcpy(buffer1, "Hello");
				send(tmpfd, buffer1, 256, 0);
				//if (send(cfd, buffer1, 256, 0) == -1)
				//	return print_error(__FILE__, __LINE__, std::strerror(errno), errno);
			}//deja connecté
			else {
				char	buffer[256];
				std::memset(&buffer, 0, 256);
				int rd = recv(events[i].data.fd, buffer, 256, 0);
				std::cout << buffer << std::endl;
				//if (rd == -1) // error
				if (rd == 0) {
					close(events[i].data.fd);
					clients.erase(events[i].data.fd);
				}
				//do_use_fd(events[n].data.fd);
				//std::cout << "else" << std::endl;
			}
		}
	}
}

Server::~Server() {
	if (fd != -1)
		close(fd);
	if (epollfd != -1)
		close(epollfd);
	std::map<int, Client>::iterator	it;
	for (it = clients.begin(); it != clients.end(); it++) {
		close((*it).first);
	}
}