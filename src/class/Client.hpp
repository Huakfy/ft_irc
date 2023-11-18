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
		std::string	_password;
		std::string	_real;
		int			user_fd;
		bool		auth;
		bool		registered;

	public:
		Client(struct sockaddr_in addr, socklen_t addr_size);
		~Client();
		sockaddr_in getAddr() const;

		bool	checkNickname(std::string nickname);

		void	setUsername(std::string username);
		void	setPassword(std::string password);
		void	setNickname(std::string nickname);
		void	setRealname(std::string realname);
		void	setfd(int fd);
		void	setAuth(void);
		void	setRegister(void);

		std::string	getUsername(void) const;
		std::string	getNickname(void) const;
		std::string	getRealname(void) const;
		int			getfd(void) const;
		bool		getAuth(void) const;
		bool		getRegister(void) const;
};

#endif