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

	//verifier le ports (seulement des chiffres, max, ...), port libre ?
	//mot de passe, peut etre vide ?

	try {
		Server server(argv[1], argv[2]);
		server.Launch();
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
