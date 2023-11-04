/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:16:39 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/04 15:58:15 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(char *port, char *pass) : fd(-1), epollfd(-1), addr(sockaddr_in()), _hints(addrinfo()), ev(epoll_event()) {

	(void)pass;

	_hints.ai_family = AF_INET;
	_hints.ai_socktype = SOCK_STREAM;
	_hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, port, &_hints, &_server) != 0) {
		print_error(__FILE__, __LINE__, std::strerror(errno), errno);
		throw FunctionError();
	}

	fd = socket(_server->ai_family, _server->ai_socktype, _server->ai_protocol);
	if (fd == -1) {
		freeaddrinfo(_server);
		print_error(__FILE__, __LINE__, std::strerror(errno), errno);
		throw FunctionError();
	}

	int optval = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		freeaddrinfo(_server);
		print_error(__FILE__, __LINE__, std::strerror(errno), errno);
		throw FunctionError();
	}

	if (bind(fd, _server->ai_addr, _server->ai_addrlen) == -1) {
		freeaddrinfo(_server);
		print_error(__FILE__, __LINE__, std::strerror(errno), errno);
		throw FunctionError();
	}

	if (listen(fd, MAX_EVENTS) == -1) {
		freeaddrinfo(_server);
		print_error(__FILE__, __LINE__, std::strerror(errno), errno);
		throw FunctionError();
	}
	freeaddrinfo(_server);

	epollfd = epoll_create1(0);
	if (epollfd == -1) {
		print_error(__FILE__, __LINE__, std::strerror(errno), errno);
		throw FunctionError();
	}

	ev.events = EPOLLIN;
	ev.data.fd = fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		print_error(__FILE__, __LINE__, std::strerror(errno), errno);
		throw FunctionError();
	}

	struct sigaction act, oldact;
	act.sa_handler = sigint_handler;
	if (sigemptyset(&act.sa_mask) == -1) {
		print_error(__FILE__, __LINE__, std::strerror(errno), errno);
		throw FunctionError();
	}
	act.sa_flags = 0;
	if (sigaction(SIGINT, NULL, &oldact) == -1) {
		print_error(__FILE__, __LINE__, std::strerror(errno), errno);
		throw FunctionError();
	}
	if (sigaction(SIGINT, &act, NULL) == -1) {
		print_error(__FILE__, __LINE__, std::strerror(errno), errno);
		throw FunctionError();
	}
}


void	Server::Launch() {
	while (1) {
		number_fds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (number_fds == -1) {
			print_error(__FILE__, __LINE__, std::strerror(errno), errno);
			throw FunctionError();
		}

		for (int i = 0; i < number_fds; i++) {
			//Premiere connexion
			if (events[i].data.fd == fd) {
				sockaddr_in	peer_addr = {};
				socklen_t	peer_addr_size = sizeof(peer_addr);
				int	tmpfd = accept(fd, (sockaddr *) &peer_addr, &peer_addr_size);
				if (tmpfd == -1) {
					print_error(__FILE__, __LINE__, std::strerror(errno), errno);
					throw FunctionError();
				}

				clients.insert(std::pair<int, Client>(fd, Client(peer_addr, peer_addr_size)));

				if (fcntl(tmpfd, F_SETFL, O_NONBLOCK) == -1) {
					print_error(__FILE__, __LINE__, std::strerror(errno), errno);
					throw FunctionError();
				}

				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = tmpfd;

				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, tmpfd, &ev) == -1) {
					print_error(__FILE__, __LINE__, std::strerror(errno), errno);
					throw FunctionError();
				}

				std::cout << "connected" << std::endl;
				char *ip;
				ip = inet_ntoa(clients.end()->second.getAddr().sin_addr);
				std::cout << "ip: " << ip << " port: " << ntohs(clients.end()->second.getAddr().sin_port) << std::endl;

				char buffer1[256], buffer2[256];
				std::memset(&buffer2, 0, 256);
				if (recv(tmpfd, buffer2, 256, 0) == -1 && errno != EAGAIN) {// verif pour eagain
					print_error(__FILE__, __LINE__, std::strerror(errno), errno);
					throw FunctionError();
				}
				std::cout << "Client : " << buffer2 << std::endl;

				std::memset(&buffer1, 0, 256);
				strcpy(buffer1, "Hello");
				if (send(tmpfd, buffer1, 256, 0) == -1) {
					print_error(__FILE__, __LINE__, std::strerror(errno), errno);
					throw FunctionError();
				}
			}//deja connecté
			else {
				char	buffer[256];
				std::memset(&buffer, 0, 256);
				int rd = recv(events[i].data.fd, buffer, 256, 0);
				if (rd == -1 && errno != EAGAIN) {
					print_error(__FILE__, __LINE__, std::strerror(errno), errno);
					throw FunctionError();
				}
				else if (rd == 0) {
					close(events[i].data.fd);
					clients.erase(events[i].data.fd);
				}
				std::cout << buffer << std::endl;
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