/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 14:30:28 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/27 17:39:28 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::join(std::vector<std::string> &args, Client *client){
	printlog("Entering JOIN func", LOGS);

	if (args.size() == 1)
		return log_send("461 :Not Enough parameters" + CRLF, client->getfd());

	if (args[1] == "0"){
		std::map<std::string, Channel *>::iterator it = channels.begin();
		while (it != channels.end()){
			if (it->second->isOnChannel(client->getNickname())){
				std::vector<std::string> tmp;
				tmp.push_back("PART");
				tmp.push_back(it->second->getName());
				part(tmp, client);
				it = channels.begin();
				continue;
			}
			it++;
		}
		args.erase(args.begin());
	}

	args.erase(args.begin());
	std::vector<std::string>	chans = parseArgs(args[0]);
	std::vector<std::string>	passwords;
	if (args.size() == 2) // pour que cette ligne fonctionne
		passwords = parseArgs(args[1]);
	while (passwords.size() != chans.size())
		passwords.push_back("");

	for (size_t i = 0; i < chans.size(); ++i){
		std::map<std::string, Channel *>::iterator it = channels.find(chans[i]);
		std::string	reply;
		// channel existe pas
		if (it == channels.end()){
			try{
				channels.insert(std::pair<std::string, Channel *>(chans[i], new Channel(chans[i], client->getNickname(), passwords[i], *client)));
			}
			catch (std::exception &e){
				log_send("476 " + client->getNickname() + " " + chans[i] + " :Invalid channel name" + CRLF, client->getfd());
				continue;
			}
			reply = ":" + client->getNickname() + " JOIN " + chans[i] + CRLF;
			log_send(reply, 0);
			channels[chans[i]]->broadcast(reply);
		}

		// channel existe gg à toi t'as bien suivi xD
		else{
			// faut faire des verifs ici avec les password et tout ça tmtc
			// ERR_CHANNELISFULL (471) channel remplie
			if (it->second->getMaxUser() != -1 && it->second->getCurrentUser() >= it->second->getMaxUser()){
				log_send("471 " + client->getNickname() + " " + chans[i] + " :Cannot join channel (channel full)" + CRLF, client->getfd());
				continue;
			}
			// ERR_INVITEONLYCHAN (473) channel invite mode only but user not invited
			if (it->second->getOnlyInvite() && !it->second->isInvited(client->getNickname())){
				log_send("473 " + client->getNickname() + " " + chans[i] + " :Cannot join channel (not invited)" + CRLF, client->getfd());
				continue;
			}
			// ERR_BADCHANNELKEY (475) mauvais mdp
			if (it->second->getNeedPass() && !it->second->checkPass(passwords[i])){
				log_send("475 " + client->getNickname() + " " + chans[i] + " :Cannot join channel (invalid password)" + CRLF, client->getfd());
				continue;
			}
			else{
				it->second->addMember(*client);
				if (it->second->getOnlyInvite() && it->second->isInvited(client->getNickname()))
					it->second->remInvited(client->getNickname());
				reply = ":" + client->getNickname() + "! JOIN " + chans[i] + CRLF;
				log_send(reply, 0);
				it->second->broadcast(reply);
			}
		}
		if (!channels[chans[i]]->getTopic().empty()){
			std::vector<std::string> top;
			top.push_back("TOPIC");
			top.push_back(chans[i]);
			topic(top, client);
		}

		log_send("353 " + client->getNickname() + " = " + chans[i] + " :" + channels[chans[i]]->getNameList() + CRLF, client->getfd());
		log_send("366 " + client->getNickname() + " " + chans[i] + " : End of /NAMES list" + CRLF, client->getfd());
	}
}