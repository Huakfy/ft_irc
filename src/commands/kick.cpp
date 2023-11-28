/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 14:30:32 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/28 15:48:16 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::kick(std::vector<std::string> &args, Client *client){
	printlog("Entering KICK func", LOGS);

	if (args.size() < 3)
		return log_send("461 :Not Enough parameters" + CRLF, client->getfd());

	std::map<std::string, Channel *>::iterator chan_it = channels.find(args[1]);
	if (chan_it == channels.end())
		return log_send("403 " + client->getNickname() + " " + args[1] + " :No such channel" + CRLF, client->getfd());

	if (!chan_it->second->isOnChannel(client->getNickname()))
		return log_send("442 " + client->getNickname() + " :You're not on that channel" + CRLF, client->getfd());

	if (!chan_it->second->isOp(client->getNickname()))
		return log_send("482 " + client->getNickname() + " " + chan_it->second->getName() + " :You're not channel operator" + CRLF, client->getfd());

	std::string					chan(args[1]);
	args.erase(args.begin());
	std::vector<std::string>	users = parseArgs(args[1]);
	args.erase(args.begin());
	args.erase(args.begin());
	std::string					msg = rebuilt(args);

	if (msg.size() == 1)
		msg = ":User has been kicked";
	if (msg[0] != ':')
		msg = ":" + msg;
	printlog("Message = " + msg, LOGS);
	for (std::vector<std::string>::iterator user = users.begin(); user != users.end(); ++user){
		if (!chan_it->second->isOnChannel(*user))
			log_send("441 " + client->getNickname() + " " + *user + " " + chan_it->second->getName() + " :They aren't on that channel" + CRLF, client->getfd());
		else{
			Client *other = getClientByNickname(*user);
			std::vector<std::string> tmp;
			tmp.push_back("PART");
			tmp.push_back(chan_it->second->getName());
			part(tmp, other);
			tmp.clear();
			if (channels.find(chan) != channels.end())
				chan_it->second->broadcast(":" + client->getNickname() + "! KICK " + chan_it->second->getName() + " " + *user + " " + msg + CRLF);
			log_send(":" + client->getNickname() + "! KICK " + chan + " " + *user + " " + msg + CRLF, other->getfd());
		}
	}
}
