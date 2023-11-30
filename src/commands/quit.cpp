/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: echapus <echapus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 14:31:36 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/30 19:15:44 by echapus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::quit(std::vector<std::string> &args, Client *client){
	printlog("Entering QUIT func", LOGS);
	std::vector<std::string>	channel_names;

	args.erase(args.begin());
	std::string reply = ":" + client->getNickname() + "! QUIT :Quit: " + rebuilt(args).erase(0, 1) + CRLF;
	for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
		if (it->second->isOnChannel(client->getNickname()))
			channel_names.push_back(it->second->getName());

	for (std::vector<std::string>::iterator chan_it = channel_names.begin(); chan_it != channel_names.end(); ++chan_it){
		std::map<std::string, Channel *>::iterator it = channels.find(*chan_it);
		if (it != channels.end()){
			it->second->removeMember(*client);
			it->second->broadcastChannel(reply, client->getfd());
			if (!it->second->getCurrentUser()){
				printlog("channel " + it->second->getName() + " has been deleted", LOGS);
				delete it->second;
				channels.erase(it);
			}
		}
	}

	DeleteClient(client->getfd());
}