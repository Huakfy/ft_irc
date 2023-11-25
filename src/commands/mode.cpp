#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::mode(std::vector<std::string> &args, Client *client){ std::cout << "<mode>" << std::endl; (void)args; (void)client;}
