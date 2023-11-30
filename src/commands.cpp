/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: echapus <echapus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 14:30:53 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/30 16:47:50 by echapus          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::whois(std::vector<std::string> &args, Client *client){
	return;
	printlog("Entering WHOIS func", LOGS);
	if (args.size() != 2)
		return log_send("461 " + client->getNickname() + " WHOIS :Not Enough parameters" + CRLF, client->getfd());

	std::string reply;
	if (args[1] == client->getNickname()){
		reply = "311 " + client->getNickname() + " " + client->getUsername() + " * :" + client->getRealname() + CRLF;
		reply += "318 " + client->getNickname() + " :End of WHOIS list" + CRLF;
		send(client->getfd(), reply.c_str(), reply.size(), 0);
	}
	else{
		reply = "401 " + client->getNickname() + " :No such nick" + CRLF;
		reply += "318 " + client->getNickname() + " :End of WHOIS list" + CRLF;
		send(client->getfd(), reply.c_str(), reply.size(), 0);
	}
	printlog(reply, SEND);
}

void	Server::pong(std::vector<std::string> &args, Client *client){
	printlog("Entering PONG func", LOGS);
	args.erase(args.begin());
	std::string reply = rebuilt(args);
	log_send("PONG " + reply + CRLF, client->getfd());
}

void	Server::parse_command(std::string str, Client *client){
	std::istringstream	iss(str);
	std::string			line;
	std::map<std::string, Server::Command>	cmdMap;

	cmdMap["MODE"]		= &Server::mode;
	cmdMap["KICK"]		= &Server::kick;
	cmdMap["PART"]		= &Server::part;
	cmdMap["JOIN"]		= &Server::join;
	cmdMap["USER"]		= &Server::user;
	cmdMap["PASS"]		= &Server::pass;
	cmdMap["NICK"]		= &Server::nick;
	cmdMap["PING"]		= &Server::pong;
	cmdMap["QUIT"]		= &Server::quit;
	cmdMap["TOPIC"]		= &Server::topic;
	cmdMap["WHOIS"]		= &Server::whois;
	cmdMap["INVITE"]	= &Server::invite;
	cmdMap["PRIVMSG"]	= &Server::privmsg;

	while (std::getline(iss, line)){
		if (line.rfind("CAP", 0) == 0)
			continue;

		std::vector<std::string>	args = split(line);
		if (args.empty())
			continue;
		std::string					cmd = args[0];

		// debug_buff(args);

		std::map<std::string, Server::Command>::iterator it = cmdMap.find(cmd); //comme une envie de mettre 'auto'
		if (it != cmdMap.end()){
			if (cmd == "PASS" || (client->getAuth() && cmd == "NICK") || client->getRegister()){
				try{ (this->*(cmdMap[cmd]))(args, client); }
				catch (std::exception &e) { static_cast<void>(e); break;}
			}
			else{
				std::string error;
				if (!client->getAuth() && cmd == "NICK")
					error = "464 " + client->getNickname() + " :Incorrect password" + CRLF;
				else
					error = "451 " + client->getNickname() + " :You have not been registered" + CRLF;
				log_send(error, client->getfd());
				DeleteClient(client->getfd());
			}
		}
	}
}