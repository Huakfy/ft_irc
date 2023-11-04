/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:16:34 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/04 11:02:38 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "../Include.hpp"
# include "Client.hpp"
# include <map>

# define MAX_EVENTS 10

class Server {
	private:
		int	fd, epollfd, number_fds;
		struct sockaddr_in	addr;
		struct addrinfo _hints;
		struct addrinfo *_server;
		std::map<int, Client>	clients;
		struct epoll_event	ev, events[MAX_EVENTS];

		class GetaddrinfoError : public std::exception{
			public:
				virtual const char * what() const throw(){
					return ("getaddrinfo error");
				}
		};

		class SocketError : public std::exception{
			public:
				virtual const char * what() const throw(){
					return ("socket error");
				}
		};

		class SetsockoptError : public std::exception{
			public:
				virtual const char * what() const throw(){
					return ("setsockopt error");
				}
		};

		class BindError : public std::exception{
			public:
				virtual const char * what() const throw(){
					return ("bind error");
				}
		};

		class ListenError : public std::exception{
			public:
				virtual const char * what() const throw(){
					return ("listen error");
				}
		};

	public:
		Server(char *port, char *pass);
		void	Launch();
		~Server();

};

#endif