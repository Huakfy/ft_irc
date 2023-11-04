/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:45:18 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/04 11:25:16 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "../Include.hpp"

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