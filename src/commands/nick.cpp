/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 14:31:53 by mjourno           #+#    #+#             */
/*   Updated: 2024/04/02 16:51:45 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

static void	printMoTD(Client &client){
	std::string nick = client.getNickname();
	int fd = client.getfd();
	log_send("375 " + nick + ": -Message of the day-" + CRLF, fd);
	log_send("372 " + nick + " :" + CRLF, fd);
	log_send("372 " + nick + " : ███████░████████▒     ██░██████░  ██████░" + CRLF, fd);
	log_send("372 " + nick + " : ██░░░░░░░░░██░░░░     ██░██░░░██░██░░░░░░" + CRLF, fd);
	log_send("372 " + nick + " : █████░     ██░        ██░██████░░██░     " + CRLF, fd);
	log_send("372 " + nick + " : ██░░░░     ██░        ██░██░░░██░██░     " + CRLF, fd);
	log_send("372 " + nick + " : ██░        ██░███████░██░██░  ██░░██████░" + CRLF, fd);
	log_send("372 " + nick + " : ░░░        ░░░░░░░░░░░░░░░░  ░░░ ░░░░░░░" + CRLF, fd);
	log_send("372 " + nick + " :" + CRLF, fd);
	log_send("376 " + nick + " : End of /MOTDc command." + CRLF, fd);
}

void	Server::nick(std::vector<std::string> &args, Client *client){
	printlog("Entering NICK func", LOGS);
	std::string error;
	if (args.size() == 1 || !client->checkNickname(args[1])){
		if (args.size() == 1)
			error = "431 : No nickname given" + CRLF;
		else
			error = "432 : Erroneus nickname" + CRLF;
		log_send(error, client->getfd());
		throw FunctionError();
	}

	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it){
		if (it->second->getNickname() == args[1]){
			error = "433 : Nickname alredy in use" + CRLF;
			log_send(error, client->getfd());
			throw FunctionError();
		}
	}

	std::string	truncedNickname = args[1];
	if (truncedNickname.size() > 9)
		truncedNickname = truncedNickname.substr(0, 9);

	if (client->getNickname().empty()){
		std::string welcome = "001 " + truncedNickname + " :Welcome to our Server. A echapus & mjourno network !" + CRLF; // on grade l'espace avant les ':' mais pas celui après pour que ça soit plus propre
		log_send(welcome, client->getfd());
		printMoTD(*client);
	}
	else{
		std::string reply = ":" +  client->getNickname() + " NICK " + truncedNickname + CRLF;
		printlog(reply, LOGS);
		for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); it++)
			send(it->first, reply.c_str(), reply.size(), 0);
		for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it){
			if (it->second->isOnChannel(client->getNickname()))
				it->second->modeNickname(*client, truncedNickname);
			if (it->second->isInvited(client->getNickname()))
				it->second->invitedChangeNickname(*client, truncedNickname);
		}
	}
	client->setNickname(truncedNickname);
	client->setRegister();
}
