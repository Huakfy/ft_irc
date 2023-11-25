#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::user(std::vector<std::string> &args, Client *client){
	printlog("Entering USER func", LOGS);
	if (args.size() < 5)
		return log_send("461 " + client->getNickname() + " USER :Not Enough parameters" + CRLF, client->getfd());
	client->setUsername(args[1]);
	std::string realname;
	for (unsigned int index = 4; index < args.size(); ++index)
		realname += args[index];
	client->setRealname(realname);
}