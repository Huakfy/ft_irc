/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/18 11:56:32 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/24 14:33:20 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include "Server.hpp"

int	main(int argc, char **argv) {
	if (argc != 3)
		return print_error(__FILE__, __LINE__, "Needs two arguments ex: ./ircserv <port> <password>", 1);

	unsigned int	i = 0;
	while (argv[1][i])
		if (!std::isdigit(argv[1][i++]))
			break;
	if (i != std::strlen(argv[1]) || std::atoi(argv[1]) > 65535 || std::atoi(argv[1]) < 0)
		return print_error(__FILE__, __LINE__, "Port needs to be a postive integer between 0 and 65535", 1);

	try {
		Server server(argv[1], argv[2]);
		server.Launch();
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
