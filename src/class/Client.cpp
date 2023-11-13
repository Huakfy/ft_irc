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

Client::Client(struct sockaddr_in address, socklen_t address_size) : addr(sockaddr_in()), addr_size(sizeof(addr)), welcome(false) {
	addr = address;
	addr_size = address_size;
}

Client::~Client() {

}

sockaddr_in Client::getAddr() const {
	return addr;
}

void Client::Welcomed(void) { welcome = true; }

bool Client::getWelcome(void) const { return welcome; }

void	Client::setUsername(std::string username) { _username = username; }
void	Client::setPassword(std::string password) { _password = password; }
void	Client::setNickname(std::string nickname) { _nickname = nickname; }

std::string	Client::getUsername(void) const { return _username; }
std::string	Client::getNickname(void) const { return _nickname; }