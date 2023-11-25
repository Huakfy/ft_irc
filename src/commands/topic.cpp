#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

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
			if (args.size() > 2 && it->second->getT() && !it->second->isOp(client->getNickname())){
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