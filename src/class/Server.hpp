/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:16:34 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/23 15:06:32 by mjourno          ###   ########.fr       */
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
		struct addrinfo _hints;
		struct addrinfo *_server;
		std::map<int, Client*>	clients;
		std::map<std::string, Channel*> channels;
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
		void	ExistingClient(int user_fd);

		typedef void (Server::*Command)(std::vector<std::string> &buffer, Client *client);
		void	mode(std::vector<std::string> &buffer, Client *client);
		void	kick(std::vector<std::string> &buffer, Client *client);
		void	part(std::vector<std::string> &buffer, Client *client);
		void	join(std::vector<std::string> &buffer, Client *client);
		void	user(std::vector<std::string> &buffer, Client *client);
		void	pass(std::vector<std::string> &buffer, Client *client);
		void	privmsg(std::vector<std::string> &buffer, Client *client);
		void	invite(std::vector<std::string> &buffer, Client *client);
		void	topic(std::vector<std::string> &buffer, Client *client);
		void	nick(std::vector<std::string> &buffer, Client *client);
		void	whois(std::vector<std::string> &buffer, Client *client);
		void	pong(std::vector<std::string> &buffer, Client *client);
		void	parse_command(std::string buffer, Client *client);

	public:
		Server(char *port, char *pass);
		void	Launch();
		~Server();

};

#endif