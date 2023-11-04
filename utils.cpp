/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:28:02 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/04 16:23:51 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Include.hpp"

int	print_error(std::string file, int line, std::string error, int err) {
	std::cerr << file << " line " << line - 1 << ": " << error << std::endl;
	return err;
}

void	sigint_handler(int signum) {
	throw SigInt();
	(void)signum;
}
