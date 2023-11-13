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
