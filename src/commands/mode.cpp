/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 14:30:36 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/27 18:48:32 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

//i https://modern.ircdocs.horse/#invite-only-channel-mode	D
//t https://modern.ircdocs.horse/#protected-topic-mode		D
//k https://modern.ircdocs.horse/#key-channel-mode			C
//o https://modern.ircdocs.horse/#operator-prefix			B
//l https://modern.ircdocs.horse/#client-limit-channel-mode	C

static bool	check_int(std::string str){
	for (int i = 0; str[i]; i++){
		if (!std::isdigit(str[i]))
			return 0;
		if (i > 9)
			return 0;
	}
	long result = std::atol(str.c_str());
	if (result > 2147483647)
		return 0;
	return	1;
}

static void	channelMode(std::vector<std::string> &args, Client *client, Channel *channel){
	std::string reply = "324 " + client->getNickname() + " " + args[1] + " ";
	std::string plus;
	std::string minus;
	channel->getOnlyInvite() ? plus += "i" : minus += "i";
	channel->getT() ? plus += "t" : minus += "t";
	channel->getNeedPass() ? plus += "k" : minus += "k";
	channel->getMaxUser() != -1 ? plus += "l" : minus += "l";
	if (!plus.empty())
		reply += "+" + plus;
	if (!minus.empty())
		reply += "-" + minus;
	std::stringstream ss;
	if (channel->getMaxUser() != -1) {
		ss << channel->getMaxUser();
		reply += " " + ss.str();
	}
	log_send(reply, client->getfd());

	ss.clear();
	ss << channel->getCreationTime().tv_sec;
	log_send("329 " + client->getNickname() + " " + args[1] + " " + ss.str(), client->getfd());
}

void	Server::mode(std::vector<std::string> &args, Client *client){
	printlog("Entering MODE func", LOGS);

	if (args.size() < 2) //MODE
		return log_send("461 " + client->getNickname() + " MODE :Not enough parameters", client->getfd()); // not enough params ?
	if (args[1][0] != '#')
		return;
	if (channels.find(args[1]) == channels.end())
		return log_send("403 " + client->getNickname() + " " + args[1] + " :No such channel" + CRLF, client->getfd());
	Channel *channel = channels.find(args[1])->second;
	if (args.size() == 2)
		return (channelMode(args, client, channel));
	if (!channel->isOp(client->getNickname()))
		return log_send("482 " + client->getNickname() + " " + args[1] + " :You're not channel operator" + CRLF, client->getfd());


	std::vector<std::string>::iterator	it = args.begin() + 3;
	bool positive = true;
	std::string modestring = args[2];
	if (modestring[0] != '+' && modestring[0] != '-')
		return printlog("Mode doesn't start with + or - dropping command", LOGS);
	for (int i = 0; modestring[i]; i++){
		if (modestring[i] == '+')
			positive = true;
		else if (modestring[i] =='-')
			positive = false;
		else if (modestring[i] == 'i'){
			if (channel->getOnlyInvite() == 0 && positive)
				channel->setOnlyInvite(1);
			else if (channel->getOnlyInvite() == 1 && !positive)
				channel->setOnlyInvite(0);
		}
		else if (modestring[i] == 't'){
			if (channel->getT() == 0 && positive)
				channel->setT(1);
			else if (channel->getT() == 1 && !positive)
				channel->setT(0);
		}
		else if (modestring[i] == 'k'){
			if (it != args.end()){
				if (positive){
					channel->setPass(*it);
					it++;
				}
			}
			if (channel->getNeedPass() == 1 && !positive)
				channel->setPass("");
		}
		else if (modestring[i] == 'o'){
			if (it != args.end()){
				if (!channel->isOp(*it) && positive)
					channel->setOp(*it);
				else if (channel->isOp(*it) && !positive)
					channel->removeOp(*it);
				it++;
			}
		}
		else if (modestring[i] == 'l'){
			if (it != args.end()){
				if (positive && check_int(*it)){
					channel->setMaxUser(std::atoi((*it).c_str()));
					it++;
				}
			}
			if (channel->getMaxUser() != -1 && !positive) {
				channel->setMaxUser(-1);
			}
		}
	}
}
