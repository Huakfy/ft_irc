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

bool	Client::setUsername(std::string username) {
	if (username.empty())
		return false;
	_username = username;
	return true;
}

bool	Client::setNickname(std::string nickname) {
	unsigned int	index = 0;
	std::string		must_not = " .,*?!@";
	
	std::cout << "setnickname check `" << nickname << "`" << std::endl;

	if (nickname.empty() || nickname[index] == '$' || nickname[index] == ':')
		return false;
	while (nickname[++index]){
		if (must_not.find(nickname[index]) != std::string::npos)
			return false;
	}
	_nickname = nickname;
	return true;
}

void	Client::setPassword(std::string password) { _password = password; }
void	Client::setfd(int fd) { user_fd = fd; }

std::string	Client::getUsername(void) const { return _username; }
std::string	Client::getNickname(void) const { return _nickname; }
int			Client::getfd(void) const { return user_fd; }