/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 14:31:47 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/27 14:31:48 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::part(std::vector<std::string> &args, Client *client){
	printlog("Entering PART func", LOGS);

	if (args.size() < 2)
		return log_send("461 " + client->getNickname() + " TOPIC :Not Enough parameters" + CRLF, client->getfd());

	args.erase(args.begin());
	std::vector<std::string>	partargs = parseArgs(args[0]);
	std::vector<std::string>	reasons;
	if (args.size() >= 2)
		reasons = parseArgs(args[1]);
	while (reasons.size() != partargs.size())
		reasons.push_back("");
	//No such channel + Not on channel
	for (std::vector<std::string>::iterator it = partargs.begin(); it != partargs.end(); ++it){
		std::map<std::string, Channel*>::iterator chan_it = channels.find(*it);
		if (chan_it == channels.end()){
			log_send("403 " + client->getNickname() + " " + args[1] + " :No such channel" + CRLF, client->getfd());
			continue;
		}
		else{
			if (!chan_it->second->isOnChannel(client->getNickname())){
				log_send("442 " + client->getNickname() + " " + args[1] + " :You're not on that channel" + CRLF, client->getfd());
				continue;
			}
			else{
				std::string reply = ":" + client->getNickname() + "! PART " + chan_it->second->getName();
				if (!reasons[it - partargs.begin()].empty())
					reply += " " + reasons[it - partargs.begin()];
				reply += CRLF;
				chan_it->second->broadcast(reply);
				log_send(reply, 0);
				chan_it->second->removeMember(*client);
			}
		}
		// empty channel == delete channel
		if (!chan_it->second->getCurrentUser()){
			printlog("channel " + chan_it->second->getName() + " has been deleted", LOGS);
			delete chan_it->second;
			channels.erase(chan_it);
		}
	}
}