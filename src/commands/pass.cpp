#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::pass(std::vector<std::string> &args, Client *client){
	printlog("Entering PASS func", LOGS);
	if (args.size() == 1 || args[1] != _pass){
		std::string error = "464 :Incorrect password" + CRLF; // or needmoreparams osef on pas se mytho
		printlog(error, SEND);
		send(client->getfd(), error.c_str(), error.size(), 0);
		DeleteClient(client->getfd());
		throw FunctionError();
	}
	client->setAuth();
}