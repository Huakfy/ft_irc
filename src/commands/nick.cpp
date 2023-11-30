/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: echapus <echapus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 14:31:53 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/30 19:02:15 by echapus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

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

	if (client->getNickname().empty()){
		std::string welcome = 	"001 " + args[1] + " :Welcome to our Server. A echapus & mjourno network !" + CRLF; // on grade l'espace avant les ':' mais pas celui après pour que ça soit plus propre
		log_send(welcome, client->getfd());
	}
	else{
		std::string reply = ":" +  client->getNickname() + " NICK " + args[1] + CRLF;
		printlog(reply, LOGS);
		for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); it++)
			//send(client->getfd(), reply.c_str(), reply.size(), 0); // il faudrait envoyer à tout les user qui connaissent le boug qui vient de se reNick
			send(it->first, reply.c_str(), reply.size(), 0); // il faudrait envoyer à tout les user qui connaissent le boug qui vient de se reNick
		for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
			if (it->second->isOnChannel(client->getNickname()))
				it->second->modeNickname(*client, args[1]);
	}
	client->setNickname(args[1]);
	client->setRegister();
}