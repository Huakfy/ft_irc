#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::mode(std::vector<std::string> &args, Client *client){ std::cout << "<mode>" << std::endl; (void)args; (void)client;}

void	Server::kick(std::vector<std::string> &args, Client *client){ std::cout << "<kick>" << std::endl; (void)args; (void)client;}

void	Server::part(std::vector<std::string> &args, Client *client){ std::cout << "<part>" << std::endl; (void)args; (void)client;}

void	Server::join(std::vector<std::string> &args, Client *client){ std::cout << "<join>" << std::endl; (void)args; (void)client;}

void	Server::privmsg(std::vector<std::string> &args, Client *client){ std::cout << "<privmsg>" << std::endl; (void)args; (void)client;}

void	Server::invite(std::vector<std::string> &args, Client *client){ std::cout << "<invite>" << std::endl; (void)args; (void)client;}

void	Server::topic(std::vector<std::string> &args, Client *client){ std::cout << "<topic>" << std::endl; (void)args; (void)client;}

void	Server::nick(std::vector<std::string> &args, Client *client){
	std::cout << "<nick>" << std::endl; (void)args; (void)client;
	std::string error;
	if (args.size() == 1 || !client->checkNickname(args[1])){
		if (args.size() == 1)
			error = "431 :No nickname given" + CRLF;
		else
			error = "432 :Erroneous nickname" + CRLF;
		std::cout << error;
		send(client->getfd(), error.c_str(), error.size(), 0);
		throw FunctionError();
	}

	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it){
		if (it->second->getNickname() == args[1]){
			error = "433 :Nicknamealredy in use" + CRLF;
			send(client->getfd(), error.c_str(), error.size(), 0);
			throw FunctionError();
		}
	}

	if (client->getNickname().empty()){
		std::string welcome = 	"001 " + client->getUsername() + " " + client->getNickname() +
								" : Welcome to our Server. A echapus & mjourno network !" + CRLF;
		send(client->getfd(), welcome.c_str(), welcome.size(), 0);
	}
	client->setNickname(args[1]);
	client->setRegister();
}

void	Server::user(std::vector<std::string> &args, Client *client){
	std::cout << "<user>" << std::endl; (void)args; (void)client;
	if (args.size() != 5){
		std::string error = "461 :Not Enough parameters" + CRLF;
		std::cout << error;
		send(client->getfd(), error.c_str(), error.size(), 0);
		throw FunctionError();
	}
	client->setUsername(args[1]);
	client->setRealname(args[4]);
}

void	Server::pass(std::vector<std::string> &args, Client *client){
	std::cout << "<pass>" << std::endl;
	if (args.size() == 1 || args[1] != _pass){
		std::string error = "464 :Incorrect password" + CRLF;
		std::cout << error;
		send(client->getfd(), error.c_str(), error.size(), 0);
		DeleteClient(client->getfd());
		throw FunctionError();
	}
	client->setAuth();
}

void	Server::whois(std::vector<std::string> &args, Client *client){
	std::cout << "<whois>" << std::endl;
	if (args.size() != 1){
		std::string error = "461 :Not enough parameters" + CRLF;
		std::cout << error;
		send(client->getfd(), error.c_str(), error.size(), 0);
		throw FunctionError();
	}
	std::string reply;
	if (args[1] == client->getNickname()){
		reply = "311 " + client->getUsername() + " * :" + client->getRealname() + CRLF;
		reply += "318" + client->getNickname() + " :End of WHOIS list" + CRLF;
		send(client->getfd(), reply.c_str(), reply.size(), 0);
	}
	else{
		reply = "401 " + client->getNickname() + ":No such nick" + CRLF;
		reply += "318" + client->getNickname() + " :End of /WHOIS list" + CRLF;
		send(client->getfd(), reply.c_str(), reply.size(), 0);
	}
}

void	Server::pong(std::vector<std::string> &args, Client *client){
	args.clear();
	std::cout << "<pong>" << std::endl;
	std::string reply = "PONG " + client->getNickname() + CRLF;
	send(client->getfd(), reply.c_str(), reply.size(), 0);
}

void	debug_buff(std::vector<std::string> buff){
	std::cout << "debug" << std::endl;
	for (std::vector<std::string>::iterator it = buff.begin(); it != buff.end(); ++it)
		std::cout << "*" << *it;
	std::cout << std::endl;
}

void	Server::parse_command(std::string args, Client *client){
	std::istringstream	iss(args);
	std::string			line;
	std::map<std::string, Server::Command>	cmdMap;

	cmdMap["MODE"]		= &Server::mode;
	cmdMap["KICK"]		= &Server::kick;
	cmdMap["PART"]		= &Server::part;
	cmdMap["JOIN"]		= &Server::join;
	cmdMap["USER"]		= &Server::user;
	cmdMap["PASS"]		= &Server::pass;
	cmdMap["NICK"]		= &Server::nick;
	cmdMap["PING"]		= &Server::pong;
	cmdMap["TOPIC"]		= &Server::topic;
	cmdMap["WHOIS"]		= &Server::whois;
	cmdMap["INVITE"]	= &Server::invite;
	cmdMap["PRIVMSG"]	= &Server::privmsg;

	while (std::getline(iss, line)){
		if (line.rfind("CAP", 0) == 0)
			continue;

		std::vector<std::string>	args = split(line);
		std::string					cmd = args[0];

		debug_buff(args);

		std::map<std::string, Server::Command>::iterator it = cmdMap.find(cmd); //comme une envie de mettre 'auto'
		if (it != cmdMap.end()){
			if (cmd == "PASS" || (client->getAuth() && cmd == "NICK") || client->getRegister()){
				try{ (this->*(cmdMap[cmd]))(args, client); }
				catch (std::exception &e) { static_cast<void>(e); }
			}
			else{
				std::string error;
				if (!client->getAuth() && cmd == "NICK")
					error = "464 " + client->getNickname() + " :Incorrect password" + CRLF;
				else
					error = "451 " + client->getNickname() + " :You have not registered" + CRLF;
				std::cout << error;
				send(client->getfd(), error.c_str(), error.size(), 0);
				DeleteClient(client->getfd());
			}
		}
	}
}