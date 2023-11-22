#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::mode(std::vector<std::string> &args, Client *client){ std::cout << "<mode>" << std::endl; (void)args; (void)client;}

void	Server::kick(std::vector<std::string> &args, Client *client){ std::cout << "<kick>" << std::endl; (void)args; (void)client;}

void	Server::part(std::vector<std::string> &args, Client *client){ std::cout << "<part>" << std::endl; (void)args; (void)client;}

void	Server::join(std::vector<std::string> &args, Client *client){
	printlog("Entering JOIN func", LOGS);

	if (args.size() == 1)
		return log_send("461 :Not Enough parameters" + CRLF, client->getfd());

	args.erase(args.begin());
	// faire JOIN 0 ? (0 permet de leave tout les channels desquels tu faisais parti en envoyant la commande PART à ta place)
	// retirer le 0 du vect si on le fait ou simplement args.erase(args.begin()) puisqu'un autre erase va être fait après quoi qu'il arrive donc 2 erase donc ce qu'on veut au final
	std::vector<std::string>	chans = parseArgs(args[1]);
	std::vector<std::string>	passwords;
	if (args.size() == 2) // pour que cette ligne fonctionne
		passwords = parseArgs(args[2]);
	while (passwords.size() != chans.size())
		passwords.push_back("");

	for (size_t i = 0; i < chans.size(); ++i){
		std::map<std::string, Channel *>::iterator it = channels.find(chans[i]);
		std::string	reply;
		// channel existe pas
		if (it == channels.end()){
			try{
				channels.insert(std::pair<std::string, Channel *>(chans[i], new Channel(chans[i], client->getNickname(), passwords[i], *client)));
			}
			catch (std::exception &e){
				log_send("476 " + client->getNickname() + " " + chans[i] + " :Invalid channel name" + CRLF, client->getfd());
				continue;
			}
			log_send(":" + client->getNickname() + " JOIN " + chans[i] + CRLF, 0);
			channels[chans[i]]->broadcast(reply);
		}

		// channel existe gg à toi t'as bien suivi xD
		else{
			// faut faire des verifs ici avec les password et tout ça tmtc
			// ERR_INVITEONLYCHAN (473) channel invite mode only but user not invited
			if (it->second->getCurrentUser() == it->second->getMaxUser()){
				log_send("473" + client->getNickname() + " " + chans[i] + " :Cannot join channel (channel full)" + CRLF, client->getfd());
				continue;
			}
			// ERR_CHANNELISFULL (471) channel remplie
			if (it->second->getOnlyInvite() && !it->second->isInvited(client->getNickname())){
				log_send("471" + client->getNickname() + " " + chans[i] + " :Cannot join channel (not invited)" + CRLF, client->getfd());
				continue;
			}
			// ERR_BADCHANNELKEY (475) mauvais mdp
			if (it->second->getNeedPass() && !it->second->checkPass(passwords[i])){
				log_send("475" + client->getNickname() + " " + chans[i] + " :Cannot join channel (invalid password)" + CRLF, client->getfd());
				continue;
			}
			else{
				it->second->addMember(*client);
				log_send(":" + client->getNickname() + " JOIN " + chans[i] + CRLF, 0);
				it->second->broadcast(reply);
			}
		}
		// enovyer le topic si topic set (je vois pas comment lancer topic autrement)
		if (!channels[chans[i]]->getTopic().empty()){
			std::vector<std::string> top;
			top.push_back("TOPIC");
			top.push_back(chans[i]);
			topic(top, client);
		}

		// https://modern.ircdocs.horse/#rplnamreply-353
		// RPL_NAMREPLY (353)
		// RPL_ENDOFNAMES (366) comme WHOIS la merde
	}
}

void	Server::privmsg(std::vector<std::string> &args, Client *client){ std::cout << "<privmsg>" << std::endl; (void)args; (void)client;}

void	Server::invite(std::vector<std::string> &args, Client *client){ std::cout << "<invite>" << std::endl; (void)args; (void)client;}

// utiliser rebuilt pour les autres params de topic tqt fait moi confiance
void	Server::topic(std::vector<std::string> &args, Client *client){ std::cout << "<topic>" << std::endl; (void)args; (void)client;}

void	Server::nick(std::vector<std::string> &args, Client *client){
	printlog("Entering NICK func", LOGS);
	std::string error;
	if (args.size() == 1 || !client->checkNickname(args[1])){
		if (args.size() == 1)
			error = "431 :No nickname given" + CRLF;
		else
			error = "432 :Erroneus nickname" + CRLF;
		log_send(error, client->getfd());
		throw FunctionError();
	}

	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it){
		if (it->second->getNickname() == args[1]){
			error = "433 :Nickname alredy in use" + CRLF;
			log_send(error, client->getfd());
			throw FunctionError();
		}
	}

	if (client->getNickname().empty()){
		std::string welcome = 	"001 " + args[1] + " :Welcome to our Server. A echapus & mjourno network !" + CRLF; // on grade l'espace avant les ':' mais pas celui après pour que ça soit plus propre
		log_send(welcome, client->getfd());
	}
	else{
		std::string reply = "!" +  client->getNickname() + " NICK " + args[1] + CRLF;
		printlog(reply, LOGS);
		send(client->getfd(), reply.c_str(), reply.size(), 0); // il faudrait envoyer à tout les user qui connaissent le boug qui vient de se reNick
	}
	client->setNickname(args[1]);
	client->setRegister();
}

void	Server::user(std::vector<std::string> &args, Client *client){
	printlog("Entering USER func", LOGS);
	if (args.size() < 5)
		return log_send("461" + client->getNickname() + " USER :Not Enough parameters" + CRLF, client->getfd());
	client->setUsername(args[1]);
	std::string realname;
	for (unsigned int index = 4; index < args.size(); ++index)
		realname += args[index];
	client->setRealname(realname);
}

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

void	Server::whois(std::vector<std::string> &args, Client *client){
	printlog("Entering WHOIS func", LOGS);
	if (args.size() != 2)
		return log_send("461" + client->getNickname() + " WHOIS :Not Enough parameters" + CRLF, client->getfd());

	std::string reply;
	if (args[1] == client->getNickname()){
		reply = "311 " + client->getNickname() + " " + client->getUsername() + " * :" + client->getRealname() + CRLF;
		reply += "318 " + client->getNickname() + " :End of WHOIS list" + CRLF;
		send(client->getfd(), reply.c_str(), reply.size(), 0);
	}
	else{
		reply = "401 " + client->getNickname() + ":No such nick" + CRLF;
		reply += "318" + client->getNickname() + " :End of WHOIS list" + CRLF;
		send(client->getfd(), reply.c_str(), reply.size(), 0);
	}
	printlog(reply, SEND);
}

void	Server::pong(std::vector<std::string> &args, Client *client){
	args.clear();
	printlog("Entering PONG func", LOGS);
	log_send("PONG " + client->getNickname() + CRLF, client->getfd());
}

void	debug_buff(std::vector<std::string> buff){
	std::cout << "debug" << std::endl;
	for (std::vector<std::string>::iterator it = buff.begin(); it != buff.end(); ++it)
		std::cout << "*" << *it;
	std::cout << std::endl;
}

void	Server::parse_command(std::string str, Client *client){
	std::istringstream	iss(str);
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
		if (args.empty())
			continue;
		std::string					cmd = args[0];

		// debug_buff(args);

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
					error = "451 " + client->getNickname() + " :You have not been registered" + CRLF;
				log_send(error, client->getfd());
				DeleteClient(client->getfd());
			}
		}
	}
}