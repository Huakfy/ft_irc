/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 11:56:32 by mjourno           #+#    #+#             */
/*   Updated: 2023/10/27 16:35:19 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

//socket
#include <sys/types.h>
#include <sys/socket.h>
//close
#include <unistd.h>
//errno
#include <cerrno>
#include <cstring>
//inet_ntoa
#include <netinet/in.h>
#include <arpa/inet.h>

int	print_error(std::string file, int line, std::string error, int err) {
	std::cerr << file << " line " << line << ": " << error << std::endl;
	return err;
}

int	main(int argc, char **argv) {
	if (argc != 3)
		//return std::cerr << "Needs two arguments ex: ./ircserv <port> <password>" << std::endl, 1;
		return print_error(__FILE__, __LINE__, "Needs two arguments ex: ./ircserv <port> <password>", 1);

	//verifier le ports (seulement des chiffres, max, ...)
	//mot de passe, peut etre vide ?

	//tests
	int	fd;
	//int socket(int domain, int type, int protocol); http://manpagesfr.free.fr/man/man2/socket.2.html
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		return print_error(__FILE__, __LINE__, std::strerror(errno), errno);
	std::cout << "fd: " << fd << std::endl;

	//int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen); https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-setsockopt-set-options-associated-socket
	//int a = setsockopt(fd, SOL_SOCKET, int optname, const void *optval, socklen_t optlen);
	//if (a == -1)
	//	return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

	//https://stackoverflow.com/questions/20941027/getsockname-c-not-setting-value
	struct sockaddr_in	name;
	socklen_t			len = sizeof(name);

	memset(&name, 0, len);
	//int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); http://manpagesfr.free.fr/man/man2/bind.2.html
	if (bind(fd, (struct sockaddr *) &name, len) == -1)
		return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

	memset(&name, 42, len);
	//int getsockname(int s, struct sockaddr *name, socklen_t *namelen); http://manpagesfr.free.fr/man/man2/getsockname.2.html
	if (getsockname(fd, (struct sockaddr *)&name, &len) == -1)
		return print_error(__FILE__, __LINE__, std::strerror(errno), errno);
	std::cout << "address name: " << inet_ntoa(name.sin_addr) << std::endl;

	if (close(fd) == -1)
		return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

	(void)argv;
	return 0;
}