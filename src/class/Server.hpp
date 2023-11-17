/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: echapus <echapus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:16:34 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/06 17:31:37 by echapus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "ft_irc.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include <map>

# define MAX_EVENTS 10

class Server {
	private:
		int	fd, epollfd, number_fds;
		// struct sockaddr_in	addr;
		struct addrinfo _hints;
		struct addrinfo *_server;
		std::map<int, Client*>	clients;
		std::map<std::string, Channel> channels;
		struct epoll_event	ev, events[MAX_EVENTS];
		std::string _buffer;
		std::string _pass;

		class FunctionError : public std::exception{
			public:
				virtual const char * what() const throw(){
					return ("Function failed");
				}
		};

		void	PrintFunctionError(std::string file, int line, std::string error, int err);
		int		NewClient(void);
		void	DeleteClient(int user_fd);
		bool	FillBuffer(int user_fd);
		bool	GetClientInfo(int user_fd, std::string message);
		void	ExistingClient(int user_fd);

	public:
		Server(char *port, char *pass);
		void	Launch();
		~Server();

};

#endif