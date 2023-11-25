#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::kick(std::vector<std::string> &args, Client *client){ std::cout << "<kick>" << std::endl; (void)args; (void)client;}
