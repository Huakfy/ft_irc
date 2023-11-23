/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:28:02 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/23 17:43:45 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

int	print_error(std::string file, int line, std::string error, int err) {
	std::cerr << file << " line " << line - 1 << ": " << error << std::endl;
	return err;
}

void	sigint_handler(int signum) {
	throw SigInt();
	(void)signum;
}

std::vector<std::string>	split(std::string str){
	std::vector<std::string>	spl;
	std::istringstream			iss(str);
	std::string					word;

	while (iss >> word)
		spl.push_back(word);
	return spl;
}

/*
	concatenate every string inside args into one string between space
*/
std::string	rebuilt(std::vector<std::string> args){
	std::string	str;
	for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); ++it)
		str += *it + " ";
	str.erase(str.size() - 1, 1);
	return str;
}

std::vector<std::string>	parseArgs(std::string msg){
	std::vector<std::string>	args;
	std::istringstream			iss(msg);
	std::string					word;

	while(std::getline(iss, word, ','))
		args.push_back(word);
	return (args);
}

void	log_send(std::string str, int fd){
	if (str.rfind(CRLF) == std::string::npos)
		str += CRLF;
	printlog(str, SEND);
	if (fd)
		send(fd, str.c_str(), str.size(), 0);
}

/*
	mode : RECV after recv func, SEND before send func, LOGS for other
*/
void	printlog(std::string msg, int mode){
	std::string server;
	if (mode == RECV)
		server = "\033[0;92m<Server RECV>"; // GREEN
	else if (mode == SEND)
		server = "\033[0;94m<Server SEND>"; // BLUE
	else
		server = "\033[0;91m<SERVER LOGS>"; // RED

	std::cout << server << "\033[0;39m " << msg; // RESET color
	if (msg.find('\n') == std::string::npos)
		std::cout << std::endl;
}