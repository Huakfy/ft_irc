/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 14:31:36 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/27 14:31:36 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::quit(std::vector<std::string> &args, Client *client){
	printlog("Entering QUIT func", LOGS);

	args.erase(args.begin());
	std::string reply = ":" + client->getNickname() + "! QUIT :Quit: " + rebuilt(args).erase(0, 1) + CRLF;
	for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
		if (it->second->isOnChannel(client->getNickname()))
			it->second->broadcastChannel(reply, client->getfd());

	DeleteClient(client->getfd());
	(void)args;
	(void)client;
}