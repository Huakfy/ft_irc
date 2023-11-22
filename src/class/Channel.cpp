#include "Channel.hpp"

Channel::Channel(std::string name, std::string username, std::string pass, Client &client){
	if (name.find(' ') != std::string::npos || name.find('\a') != std::string::npos || name.find(',') != std::string::npos)
		throw (Channel::InvalidName());
	_name = name;
	_topic = "";
	_password = pass;
	_needpass = false;
	if (pass != "")
		_needpass = true;
	_op.push_back(username);
	_members.push_back(username);
	_usersFd.push_back(client.getfd());
	_onlyinvite = false;
	_currentUser = 1;
	_maxUser = 10;
	return ;
}

Channel::~Channel(void){
	_members.clear();
	_op.clear();
	_invited.clear();
	_usersFd.clear();
	std::cout << "Channel \"" << _name << "\" has been deleted." << std::cout;
	return ;
}

std::string	Channel::getTopic(void) const { return _topic; }
std::string	Channel::getName(void) const { return _name; }
std::string	Channel::getPass(void) const { return _password; }

unsigned int	Channel::getMaxUser(void) const { return _maxUser; }
unsigned int	Channel::getCurrentUser(void) const { return _currentUser; }

bool	Channel::getNeedPass(void) const { return _needpass; }
bool	Channel::getOnlyInvite(void) const { return _onlyinvite; }

void	Channel::addInvited(std::string username){ _invited.push_back(username); }

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
	std::cout << "User " << nickname << "[" << fd << "] has been removed" << std::endl;
}