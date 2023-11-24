#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

void	Server::mode(std::vector<std::string> &args, Client *client){ std::cout << "<mode>" << std::endl; (void)args; (void)client;}

void	Server::kick(std::vector<std::string> &args, Client *client){ std::cout << "<kick>" << std::endl; (void)args; (void)client;}

void	Server::part(std::vector<std::string> &args, Client *client){
	printlog("Entering PART func", LOGS);

	if (args.size() < 2)
		return log_send("461 " + client->getNickname() + " TOPIC :Not Enough parameters" + CRLF, client->getfd());

	args.erase(args.begin());
	std::vector<std::string>	partargs = parseArgs(args[0]);
	std::vector<std::string>	reasons;
	if (args.size() >= 2)
		reasons = parseArgs(args[1]);
	while (reasons.size() != partargs.size())
		reasons.push_back("");
	//No such channel + Not on channel
	for (std::vector<std::string>::iterator it = partargs.begin(); it != partargs.end(); ++it){
		std::map<std::string, Channel*>::iterator chan_it = channels.find(*it);
		if (chan_it == channels.end()){
			log_send("403 " + client->getNickname() + " " + args[1] + " :No such channel" + CRLF, client->getfd());
			continue;
		}
		else{
			if (!chan_it->second->isOnChannel(client->getNickname())){
				log_send("442 " + client->getNickname() + " " + args[1] + " :You're not on that channel" + CRLF, client->getfd());
				continue;
			}
			else{
				std::string reply = ":" + client->getNickname() + "! PART " + chan_it->second->getName() + CRLF;
				chan_it->second->broadcast(reply);
				log_send(reply, 0);
				chan_it->second->removeMember(*client);
			}
		}
		// empty channel == delete channel
		if (!chan_it->second->getCurrentUser()){
			printlog("channel " + chan_it->second->getName() + " has been deleted", LOGS);
			delete chan_it->second;
			channels.erase(chan_it);
		}
	}
}

void	Server::join(std::vector<std::string> &args, Client *client){
	printlog("Entering JOIN func", LOGS);

	if (args.size() == 1)
		return log_send("461 :Not Enough parameters" + CRLF, client->getfd());

	if (args[1] == "0"){
		std::map<std::string, Channel *>::iterator it = channels.begin();
		while (it != channels.end()){
			if (it->second->isOnChannel(client->getNickname())){
				std::vector<std::string> tmp;
				tmp.push_back("PART");
				tmp.push_back(it->second->getName());
				part(tmp, client);
				it = channels.begin();
				continue;
			}
			it++;
		}
		args.erase(args.begin());
	}

	args.erase(args.begin());
	// faire JOIN 0 ? (0 permet de leave tout les channels desquels tu faisais parti en envoyant la commande PART à ta place)
	// retirer le 0 du vect si on le fait ou simplement args.erase(args.begin()) puisqu'un autre erase va être fait après quoi qu'il arrive donc 2 erase donc ce qu'on veut au final
	std::vector<std::string>	chans = parseArgs(args[0]);
	std::vector<std::string>	passwords;
	if (args.size() == 2) // pour que cette ligne fonctionne
		passwords = parseArgs(args[1]);
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
			reply = ":" + client->getNickname() + " JOIN " + chans[i] + CRLF;
			log_send(reply, 0);
			channels[chans[i]]->broadcast(reply);
		}

		// channel existe gg à toi t'as bien suivi xD
		else{
			// faut faire des verifs ici avec les password et tout ça tmtc
			// ERR_CHANNELISFULL (471) channel remplie
			if (it->second->getCurrentUser() == it->second->getMaxUser()){
				log_send("471" + client->getNickname() + " " + chans[i] + " :Cannot join channel (channel full)" + CRLF, client->getfd());
				continue;
			}
			// ERR_INVITEONLYCHAN (473) channel invite mode only but user not invited
			if (it->second->getOnlyInvite() && !it->second->isInvited(client->getNickname())){
				log_send("471" + client->getNickname() + " " + chans[i] + " :Cannot join channel (not invited)" + CRLF, client->getfd());
				continue;
			}
			// ERR_BADCHANNELKEY (475) mauvais mdp
			if (it->second->getNeedPass() && !it->second->checkPass(passwords[i])){
				log_send("475 " + client->getNickname() + " " + chans[i] + " :Cannot join channel (invalid password)" + CRLF, client->getfd());
				continue;
			}
			else{
				it->second->addMember(*client);
				reply = ":" + client->getNickname() + "! JOIN " + chans[i] + CRLF;
				log_send(reply, 0);
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

		log_send("353 " + client->getNickname() + " = " + chans[i] + " :" + channels[chans[i]]->getNameList() + CRLF, client->getfd());
		log_send("366 " + client->getNickname() + " " + chans[i] + " : End of /NAMES list" + CRLF, client->getfd());
		// RPL_NAMREPLY (353)
		// RPL_ENDOFNAMES (366) comme WHOIS la merde
	}
}

void	Server::quit(std::vector<std::string> &args, Client *client){
	printlog("Entering QUIT func", LOGS);

	(void)args;
	(void)client;
}

void	Server::privmsg(std::vector<std::string> &args, Client *client){
	printlog("Entering PRIVMSG func", LOGS);

	if (args.size() == 1)
		return log_send("411 " + client->getNickname() + " :No recipient given (PRIVMSG)" + CRLF, client->getfd());
	if (args.size() == 2)
		return log_send("412 " + client->getNickname() + " :No text to send" + CRLF, client->getfd());

	args.erase(args.begin());
	std::vector<std::string>	recipient = parseArgs(args[0]);
	args.erase(args.begin());
	std::string					msg = rebuilt(args);
	msg.erase(0, 1);
	for (std::vector<std::string>::iterator target = recipient.begin(); target != recipient.end(); ++target){
		// send to channel
		if ((*target)[0] == '#'){
			if (channels.find(*target) == channels.end()){
				log_send("404 " + client->getNickname() + " " + *target + " :Cannot send to channel" + CRLF, client->getfd());
				continue;
			}
			channels[*target]->broadcastChannel(":" + client->getNickname() + " PRIVMSG " + *target + " :" + msg + CRLF, client->getfd());
		}
		// send to user
		else{
			Client *user = getClientByNickname(*target);
			if (!user){
				log_send("401 " + client->getNickname() + " :No such nick" + CRLF, client->getfd());
				continue;
			}
			log_send(":" + client->getNickname() + " PRIVMSG " + user->getNickname() + " :" + msg + CRLF, user->getfd());
		}
	}
}

void	Server::invite(std::vector<std::string> &args, Client *client){
	printlog("Entering INVITE func", LOGS);
	if (args.size() < 3)
		return log_send("461 " + client->getNickname() + " INVITE :Not enough parameters" + CRLF, client->getfd());
	if (channels.find(args[2]) == channels.end())
		return log_send("403 " + client->getNickname() + " " + args[2] + " :No such channel" + CRLF, client->getfd());
	if (channels[args[2]]->isOnChannel(client->getNickname()) == 0)
		return log_send("442 " + client->getNickname() + " " + args[2] + " :You're not on that channel" + CRLF, client->getfd());
	if (channels[args[2]]->getOnlyInvite() == 1 && channels[args[2]]->isOp(client->getNickname()) == 0)
		return log_send("482 " + client->getNickname() + " " + args[2] + " :You're not channel operator" + CRLF, client->getfd());
	if (channels[args[2]]->isOnChannel(args[1]) == 1)
		return log_send("443 " + client->getNickname() + " " + args[1] + " " + args[2] + " :is already on channel" + CRLF, client->getfd());
	Client *invited = getClientByNickname(args[1]);
	if (invited == NULL)
		return;
	channels[args[2]]->addInvited(args[1]);
	log_send("341 " + client->getNickname() + " " + args[1] + " " + args[2] + CRLF, client->getfd());
	log_send(":" + client->getNickname() + " INVITE " + args[1] + " " + args[2] + CRLF, invited->getfd());
}

void	Server::topic(std::vector<std::string> &args, Client *client){
	printlog("Entering TOPIC func", LOGS);

	//Not enough params
	if (args.size() < 2) {
		std::string error = "461 " + client->getNickname() + " TOPIC :Not Enough parameters" + CRLF;
		return log_send(error, client->getfd());
	}
	//No such channel + Not on channel + No op
	std::map<std::string, Channel*>::iterator it;
	for (it = channels.begin(); it != channels.end(); ++it){
		if (args[1] == it->first) {
			//Not on channel
			if (!(it->second->isOnChannel(client->getNickname()))){
				std::string error = "442 " + client->getNickname() + " " + args[1] + " :You're not on that channel" + CRLF;
				return log_send(error, client->getfd());
			}
			//Not op
			if (args.size() > 2 && it->second->getOnlyInvite() && !it->second->isInvited(client->getNickname())){
				std::string error = "482 " + client->getNickname() + " " + args[1] + " :You're not channel operator" + CRLF;
				return log_send(error, client->getfd());
			}
			break;
		}
	}
	//No such channel
	if (it == channels.end()) {
		std::string error = "403 " + client->getNickname() + " " + args[1] + " :No such channel" + CRLF;
		return log_send(error, client->getfd());
	}
	//checking TOPIC
	if (args.size() == 2) {
		std::string topic = it->second->getTopic();
		if (topic.empty()) {
			std::string reply = "331 " + client->getNickname() + " " + args[1] + " :No topic is set" + CRLF;
			return log_send(reply, client->getfd());
		}
		else {
			//send RPL_TOPIC + RPL_TOPICWHOTIME
			std::string reply = "332 " + client->getNickname() + " " + args[1] + " :" + topic + CRLF;
			log_send(reply, client->getfd());
			std::stringstream ss;
			ss << it->second->getTopicTime().tv_sec;
			reply = "333 " + client->getNickname() + " " + args[1] + " " + it->second->getTopicWho() + " " + ss.str() + CRLF;
			log_send(reply, client->getfd());
		}
	}
	//changer topic + Clear topic
	if (args.size() >= 3) {
		if (args[2][0] != ':')
			return;
		else {
			args.erase(args.begin());
			args.erase(args.begin());
			(args.begin())->erase(0, 1);
			std::string newTopic = rebuilt(args);
			it->second->setTopic(newTopic, *client);
		}
	}
}

void	Server::nick(std::vector<std::string> &args, Client *client){
	printlog("Entering NICK func", LOGS);
	std::string error;
	if (args.size() == 1 || !client->checkNickname(args[1])){
		if (args.size() == 1)
			error = "431 : No nickname given" + CRLF;
		else
			error = "432 : Erroneus nickname" + CRLF;
		log_send(error, client->getfd());
		throw FunctionError();
	}

	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it){
		if (it->second->getNickname() == args[1]){
			error = "433 : Nickname alredy in use" + CRLF;
			log_send(error, client->getfd());
			throw FunctionError();
		}
	}

	if (client->getNickname().empty()){
		std::string welcome = 	"001 " + args[1] + " :Welcome to our Server. A echapus & mjourno network !" + CRLF; // on grade l'espace avant les ':' mais pas celui après pour que ça soit plus propre
		log_send(welcome, client->getfd());
	}
	else{
		std::string reply = ":" +  client->getNickname() + " NICK " + args[1] + CRLF;
		printlog(reply, LOGS);
		for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); it++)
			//send(client->getfd(), reply.c_str(), reply.size(), 0); // il faudrait envoyer à tout les user qui connaissent le boug qui vient de se reNick
			send(it->first, reply.c_str(), reply.size(), 0); // il faudrait envoyer à tout les user qui connaissent le boug qui vient de se reNick
	}
	client->setNickname(args[1]);
	client->setRegister();
}

void	Server::user(std::vector<std::string> &args, Client *client){
	printlog("Entering USER func", LOGS);
	if (args.size() < 5)
		return log_send("461 " + client->getNickname() + " USER :Not Enough parameters" + CRLF, client->getfd());
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
		return log_send("461 " + client->getNickname() + " WHOIS :Not Enough parameters" + CRLF, client->getfd());

	std::string reply;
	if (args[1] == client->getNickname()){
		reply = "311 " + client->getNickname() + " " + client->getUsername() + " * :" + client->getRealname() + CRLF;
		reply += "318 " + client->getNickname() + " :End of WHOIS list" + CRLF;
		send(client->getfd(), reply.c_str(), reply.size(), 0);
	}
	else{
		reply = "401 " + client->getNickname() + " :No such nick" + CRLF;
		reply += "318 " + client->getNickname() + " :End of WHOIS list" + CRLF;
		send(client->getfd(), reply.c_str(), reply.size(), 0);
	}
	printlog(reply, SEND);
}

void	Server::pong(std::vector<std::string> &args, Client *client){
	printlog("Entering PONG func", LOGS);
	args.erase(args.begin());
	std::string reply = rebuilt(args);
	log_send("PONG " + reply + CRLF, client->getfd());
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
	cmdMap["QUIT"]		= &Server::quit;
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