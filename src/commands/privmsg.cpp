/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 14:31:40 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/27 14:31:40 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::privmsg(std::vector<std::string> &args, Client *client){
	printlog("Entering PRIVMSG func", LOGS);

	if (args.size() == 1)
		return log_send("411 " + client->getNickname() + " :No recipient given (PRIVMSG)" + CRLF, client->getfd());
	if (args.size() == 2)
		return log_send("412 " + client->getNickname() + " :No text to send" + CRLF, client->getfd());

	args.erase(args.begin());
	std::vector<std::string>	recipient = parseArgs(args[0]);
	args.erase(args.begin());
	std::string					msg = rebuilt(args);
	msg.erase(0, 1);
	for (std::vector<std::string>::iterator target = recipient.begin(); target != recipient.end(); ++target){
		// send to channel
		if ((*target)[0] == '#'){
			if (channels.find(*target) == channels.end()){
				log_send("404 " + client->getNickname() + " " + *target + " :Cannot send to channel" + CRLF, client->getfd());
				continue;
			}
			channels[*target]->broadcastChannel(":" + client->getNickname() + " PRIVMSG " + *target + " :" + msg + CRLF, client->getfd());
		}
		// send to user
		else{
			Client *user = getClientByNickname(*target);
			if (!user){
				log_send("401 " + client->getNickname() + " :No such nick" + CRLF, client->getfd());
				continue;
			}
			log_send(":" + client->getNickname() + " PRIVMSG " + user->getNickname() + " :" + msg + CRLF, user->getfd());
		}
	}
}