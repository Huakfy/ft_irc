/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: echapus <echapus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:45:18 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/14 15:57:56 by echapus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "ft_irc.hpp"

class Client {
	private:
		sockaddr_in	addr;
		socklen_t	addr_size;
		std::string	_nickname;
		std::string	_username;
		int			user_fd;
		std::string	_hostname;
		std::string	_password;
		bool		welcome;

	public:
		Client(struct sockaddr_in addr, socklen_t addr_size);
		~Client();
		sockaddr_in getAddr() const;

		bool	getWelcome(void) const;
		void	Welcomed(void);

		bool	setUsername(std::string username);
		void	setPassword(std::string password);
		bool	setNickname(std::string nickname);
		void	setfd(int fd);

		std::string	getUsername(void) const;
		std::string	getNickname(void) const;
		int			getfd(void) const;
};

#endif