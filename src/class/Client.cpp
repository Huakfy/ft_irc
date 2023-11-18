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

Client::Client(struct sockaddr_in address, socklen_t address_size) : addr(sockaddr_in()), addr_size(sizeof(addr)), auth(false), registered(false) {
	addr = address;
	addr_size = address_size;
}

Client::~Client() {

}

sockaddr_in Client::getAddr() const {
	return addr;
}


bool	Client::checkNickname(std::string nickname) {
	unsigned int	index = 0;
	std::string		must_not = " .,*?!@";
	
	if (nickname.empty() || nickname[index] == '$' || nickname[index] == ':')
		return false;
	while (nickname[++index]){
		if (must_not.find(nickname[index]) != std::string::npos)
			return false;
	}
	return true;
}
void	Client::setRealname(std::string real) {
	if (real[0] == ':')
		real.erase(0, 1);
	_real = real;
}

void	Client::setUsername(std::string username) { _username = username; }
void	Client::setNickname(std::string nickname) { _nickname = nickname; }
void	Client::setPassword(std::string password) { _password = password; }
void	Client::setfd(int fd) { user_fd = fd; }
void	Client::setAuth(void) { auth = true; }
void	Client::setRegister(void) { registered = true; }

std::string	Client::getUsername(void) const { return _username; }
std::string	Client::getNickname(void) const { return _nickname; }
std::string	Client::getRealname(void) const { return _real; }
int			Client::getfd(void) const { return user_fd; }
bool		Client::getAuth(void) const { return auth; }
bool		Client::getRegister(void) const { return registered; }