/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:45:18 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/03 15:20:55 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "../Include.hpp"

class Client {
	private:
	public:
		int	fd;
		struct sockaddr_in	addr;
		socklen_t	addr_size;
		Client();
		~Client();
};

#endif