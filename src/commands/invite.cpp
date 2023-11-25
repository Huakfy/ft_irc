#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::invite(std::vector<std::string> &args, Client *client){
	printlog("Entering INVITE func", LOGS);
	if (args.size() < 3)
		return log_send("461 " + client->getNickname() + " INVITE :Not enough parameters" + CRLF, client->getfd());
	if (channels.find(args[2]) == channels.end())
		return log_send("403 " + client->getNickname() + " " + args[2] + " :No such channel" + CRLF, client->getfd());
	if (channels[args[2]]->isOnChannel(client->getNickname()) == 0)
		return log_send("442 " + client->getNickname() + " " + args[2] + " :You're not on that channel" + CRLF, client->getfd());
	if (channels[args[2]]->getOnlyInvite() == 1 && channels[args[2]]->isOp(client->getNickname()) == 0)
		return log_send("482 " + client->getNickname() + " " + args[2] + " :You're not channel operator" + CRLF, client->getfd());
	if (channels[args[2]]->isOnChannel(args[1]) == 1)
		return log_send("443 " + client->getNickname() + " " + args[1] + " " + args[2] + " :is already on channel" + CRLF, client->getfd());
	Client *invited = getClientByNickname(args[1]);
	if (invited == NULL)
		return;
	channels[args[2]]->addInvited(args[1]);
	log_send("341 " + client->getNickname() + " " + args[1] + " " + args[2] + CRLF, client->getfd());
	log_send(":" + client->getNickname() + " INVITE " + args[1] + " " + args[2] + CRLF, invited->getfd());
}