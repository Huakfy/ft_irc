/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: echapus <echapus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:45:18 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/06 17:31:37 by echapus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "ft_irc.hpp"

class Client {
	private:
		sockaddr_in	addr;
		socklen_t	addr_size;
	public:
		Client(struct sockaddr_in addr, socklen_t addr_size);
		~Client();
		sockaddr_in getAddr() const;
};

#endif