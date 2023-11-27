/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 14:30:56 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/27 14:30:56 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::user(std::vector<std::string> &args, Client *client){
	printlog("Entering USER func", LOGS);
	if (args.size() < 5)
		return log_send("461 " + client->getNickname() + " USER :Not Enough parameters" + CRLF, client->getfd());
	client->setUsername(args[1]);
	std::string realname;
	for (unsigned int index = 4; index < args.size(); ++index)
		realname += args[index];
	client->setRealname(realname);
}