/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 11:56:32 by mjourno           #+#    #+#             */
/*   Updated: 2023/10/30 16:02:10 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

//socket
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
//close
#include <unistd.h>
//errno
#include <cerrno>
#include <cstring>
//inet_ntoa
#include <netinet/in.h>
#include <arpa/inet.h>
//getprotobyname
#include <netdb.h>

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

	int fd = socket(AF_UNIX, SOCK_STREAM, 0); //socket to listen on port
	if (fd == -1)
		return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

	struct sockaddr_un	my_addr;
	socklen_t			len = sizeof(my_addr);
	memset(&my_addr, 0, len);

	my_addr.sun_family = AF_UNIX;
	strncpy(my_addr.sun_path, "./test", sizeof(my_addr.sun_path) - 1);

	if (bind(fd, (struct sockaddr *) &my_addr, len) == -1)
		return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

	if (listen(fd, 10) == -1)
		return print_error(__FILE__, __LINE__, std::strerror(errno), errno);



	if (close(fd) == -1) //close listening fd
		return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

	(void)argv;
	return 0;
}

//int	main(int argc, char **argv) {
//	//tests
//	int	fd;
//	//int socket(int domain, int type, int protocol); http://manpagesfr.free.fr/man/man2/socket.2.html
//	fd = socket(AF_INET, SOCK_STREAM, 0);
//	if (fd == -1)
//		return print_error(__FILE__, __LINE__, std::strerror(errno), errno);
//	std::cout << "fd: " << fd << std::endl;

//	//int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen); https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-setsockopt-set-options-associated-socket
//	//int a = setsockopt(fd, SOL_SOCKET, int optname, const void *optval, socklen_t optlen);
//	//if (a == -1)
//	//	return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

//	//https://stackoverflow.com/questions/20941027/getsockname-c-not-setting-value
//	struct sockaddr_in	name;
//	socklen_t			len = sizeof(name);

//	memset(&name, 0, len);
//	//int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); http://manpagesfr.free.fr/man/man2/bind.2.html
//	if (bind(fd, (struct sockaddr *) &name, len) == -1)
//		return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

//	memset(&name, 42, len);
//	//int getsockname(int s, struct sockaddr *name, socklen_t *namelen); http://manpagesfr.free.fr/man/man2/getsockname.2.html
//	if (getsockname(fd, (struct sockaddr *)&name, &len) == -1)
//		return print_error(__FILE__, __LINE__, std::strerror(errno), errno);
//	std::cout << "address name: " << inet_ntoa(name.sin_addr) << std::endl;

//	//http://manpagesfr.free.fr/man/man3/getprotoent.3.html
//	struct protoent	*proto;
//	proto = getprotobyname(inet_ntoa(name.sin_addr));
//	if (proto == NULL)
//		std::cerr << "Error or EOF" << std::endl;

//	//gethostbyname OBSELETE -> utiliser getaddrinfo
//	//int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res); http://manpagesfr.free.fr/man/man3/getaddrinfo.3.html
//	struct addrinfo	hints, *result, *rp;
//	int				s, sfd;
//	memset(&hints, 0, sizeof(struct addrinfo));
//	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
//    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
//    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
//    hints.ai_protocol = 0;          /* Any protocol */
//    hints.ai_canonname = NULL;
//    hints.ai_addr = NULL;
//    hints.ai_next = NULL;

//	s = getaddrinfo(NULL, "", &hints, &result);
//	if (s != 0)
//		return print_error(__FILE__, __LINE__, gai_strerror(s), s);
//	/* getaddrinfo() returns a list of address structures.
//	Try each address until we successfully bind(2).
//	If socket() (or bind()) fails, we (close the socket
//	and) try the next address. */
//	for (rp = result; rp != NULL; rp = rp->ai_next) {
//		sfd = socket(rp->ai_family, rp->ai_socktype,
//				rp->ai_protocol);
//		if (sfd == -1)
//			continue;
//	//int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); http://manpagesfr.free.fr/man/man2/bind.2.html
//		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
//			break;                  /* Success */

//		close(sfd);
//	}
//	if (rp == NULL)                 /* No address succeeded */
//		return print_error(__FILE__, __LINE__, "Could not bind", 1);
//	freeaddrinfo(result);           /* No longer needed */
//	/* Read datagrams and echo them back to sender */
//	//struct sockaddr_storage peer_addr;
//	//socklen_t peer_addr_len;
//	//ssize_t nread;
//	//char buf[500];
//	//for (;;) {
//	//	peer_addr_len = sizeof(struct sockaddr_storage);
//	//	nread = recvfrom(sfd, buf, 500, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
//	//	if (nread == -1)
//	//		continue;               /* Ignore failed request */

//	//	char host[NI_MAXHOST], service[NI_MAXSERV];

//	//	s = getnameinfo((struct sockaddr *) &peer_addr, peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
//	//	if (s == 0)
//	//		std::cout << "Received " << (long)nread << " bytes from " << host << ":" << service << std::endl;
//	//	else
//	//		return print_error(__FILE__, __LINE__, gai_strerror(s), s);

//	//	if (sendto(sfd, buf, nread, 0, (struct sockaddr *) &peer_addr, peer_addr_len) != nread)
//	//		return print_error(__FILE__, __LINE__, "Error sending response", 1);
//	//}

//	//int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);


//	if (close(fd) == -1)
//		return print_error(__FILE__, __LINE__, std::strerror(errno), errno);

//	(void)argv;
//	return 0;
//}