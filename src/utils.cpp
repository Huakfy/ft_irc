/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: echapus <echapus@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:28:02 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/06 17:31:37 by echapus          ###   ########.fr       */
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