/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:45:23 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/04 11:25:59 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(struct sockaddr_in address, socklen_t address_size) : addr(sockaddr_in()), addr_size(sizeof(addr)) {
	addr = address;
	addr_size = address_size;
}

Client::~Client() {

}

sockaddr_in Client::getAddr() const {
	return addr;
}