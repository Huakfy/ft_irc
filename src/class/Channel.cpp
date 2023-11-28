/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 14:30:06 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/28 16:01:51 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string name, std::string username, std::string pass, Client &client){
	if (name.find(' ') != std::string::npos || name.find('\a') != std::string::npos || name.find(',') != std::string::npos || name.find('#') != 0)
		throw (Channel::InvalidName());
	_name = name;
	_topic = "";
	_topicWho = "";
	_topicTime = timeval();
	_password = pass;
	_needpass = false;
	if (pass != "")
		_needpass = true;
	_op.push_back(username);
	_members.push_back(username);
	_usersFd.push_back(client.getfd());
	_onlyinvite = false;
	_currentUser = 1;
	_maxUser = -1;
	_t = 0; //if 0 everyone can change topic, if one user has to be operator
	gettimeofday(&_creationTime, NULL);
	_bot = 0;
	return ;
}

Channel::~Channel(void){
	_members.clear();
	_op.clear();
	_invited.clear();
	_usersFd.clear();
	// std::cout << "Channel \"" << _name << "\" has been deleted." << std::cout;
	return ;
}

std::string	Channel::getTopic(void) const { return _topic; }

std::string	Channel::getTopicWho(void) const { return _topicWho; }

timeval	Channel::getTopicTime(void) const { return _topicTime; };

std::string	Channel::getName(void) const { return _name; }

std::string	Channel::getNameList(void) const{
	std::string	str;

	for (std::vector<std::string>::const_iterator it = _members.begin(); it != _members.end(); ++it){
		if (isOp(*it))
			str += "@";
		str += *it + " ";
	}
	if (_bot)
		str += "@IRCbot ";
	return str;
}

int	Channel::getMaxUser(void) const { return _maxUser; }
int	Channel::getCurrentUser(void) const { return _currentUser; }

bool	Channel::getNeedPass(void) const { return _needpass; }
bool	Channel::getOnlyInvite(void) const { return _onlyinvite; }

bool	Channel::checkPass(std::string pass) const { return pass == _password; }

bool	Channel::isInvited(std::string user) const { return find(_invited.begin(), _invited.end(), user) != _invited.end(); }

void	Channel::addInvited(std::string username){ _invited.push_back(username); }

void	Channel::remInvited(std::string username){
	int	i;
	for (i = 0; _invited[i] != username; ++i);
	_invited[i].erase();
}

void	Channel::addMember(Client &client){
	_members.push_back(client.getNickname());
	_usersFd.push_back(client.getfd());
	_currentUser++;
}

void	Channel::removeMember(Client &client){
	std::string	nickname = client.getNickname();
	int			fd = client.getfd();

	for(std::vector<std::string>::iterator it = _members.begin(); it != _members.end(); ++it){
		if (*it == nickname){
			_members.erase(it);
			break ;
		}
	}
	for(std::vector<std::string>::iterator it = _op.begin(); it != _op.end(); ++it){
		if (*it == nickname){
			_op.erase(it);
			break ;
		}
	}
	for(std::vector<int>::iterator it = _usersFd.begin(); it != _usersFd.end(); ++it){
		if (*it == fd){
			_usersFd.erase(it);
			break ;
		}
	}
	_currentUser--;
	std::cout << "User " << nickname << "[" << fd << "] has been removed" << std::endl;
}

void	Channel::broadcast(std::string str) {
	for (std::vector<int>::iterator it = _usersFd.begin(); it != _usersFd.end(); ++it) {
		std::stringstream ss;
		ss << *it;
		log_send("[" + ss.str() + "]" + str, 0);
		send(*it, str.c_str(), str.size(), 0);
	}
}

void	Channel::broadcastChannel(std::string str, int fd) {
	for (std::vector<int>::iterator it = _usersFd.begin(); it != _usersFd.end(); ++it) {
		if (*it == fd)
			continue;
		std::stringstream ss;
		ss << *it;
		log_send("[" + ss.str() + "]" + str, 0);
		send(*it, str.c_str(), str.size(), 0);
	}
}

bool	Channel::isOnChannel(std::string user) const {
	return find(_members.begin(), _members.end(), user) != _members.end();
}

bool	Channel::isOp(std::string user) const {
	return find(_op.begin(), _op.end(), user) != _op.end();
}

void	Channel::setTopic(std::string topic, Client &client) {
	_topic = topic;
	_topicWho = client.getNickname();
	gettimeofday(&_topicTime, NULL);

	std::string reply;
	reply = ":" + _topicWho + "!@ " + "TOPIC "  + _name + " :";
	if (!(topic.empty()))
		reply += topic;
	reply += CRLF;
	log_send(reply, 0);
	broadcast(reply);
}

bool	Channel::getT() const {return _t;}

void	Channel::setT(bool b) {_t = b;}

timeval	Channel::getCreationTime() const {return _creationTime;}

void	Channel::setOnlyInvite(bool b) { _onlyinvite = b; }

void	Channel::setPass(std::string pass) {
	_password = pass;
	_needpass = 1;
	if (pass.empty())
		_needpass = 0;
}

void	Channel::setOp(std::string name) { _op.push_back(name); }
void	Channel::removeOp(std::string name) {
	for (std::vector<std::string>::iterator it = _op.begin(); it != _op.end(); ++it)
		if (*it == name) {
			_op.erase(it);
			break;
		}
}

void	Channel::setMaxUser(int i){
	_maxUser = i;
}

bool	Channel::getBot() const{ return _bot; }

void	Channel::setBot(bool b){ _bot = b; }