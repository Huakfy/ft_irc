/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mjourno <mjourno@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/27 14:30:12 by mjourno           #+#    #+#             */
/*   Updated: 2023/11/28 15:54:42 by mjourno          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

# include "ft_irc.hpp"
# include "Client.hpp"

class Channel{
	private:
		std::vector<std::string>	_members;
		std::vector<std::string>	_op;
		std::vector<std::string>	_invited;
		std::vector<int>			_usersFd;
		std::string					_name;
		std::string					_topic;
		std::string					_topicWho;
		struct timeval				_topicTime;
		std::string					_password;
		unsigned int				_maxUser;
		unsigned int				_currentUser;
		bool						_needpass;
		bool						_onlyinvite;
		bool						_t;
		struct timeval				_creationTime;
		bool						_bot;

		class InvalidName : public std::exception{
			public:
				virtual const char * what() const throw(){
					return ("Invalid Name");
				}
		};

	public:
		Channel(std::string name, std::string username, std::string pass, Client &client);
		~Channel();

		std::string	getTopic(void) const;
		std::string	getTopicWho(void) const;
		timeval		getTopicTime(void) const;
		std::string	getName(void) const;
		std::string	getNameList(void) const;

		int			getMaxUser(void) const;
		int			getCurrentUser(void) const;
		int			getMaxUser(void) const;
		int			getCurrentUser(void) const;

		bool	getNeedPass(void) const;
		bool	getOnlyInvite(void) const;

		bool	checkPass(std::string pass) const;
		bool	isInvited(std::string user) const;

		void	addInvited(std::string username);
		void	remInvited(std::string username);
		void	remInvited(std::string username);
		void	addMember(Client &client);

		void	removeMember(Client &client);

		void	broadcast(std::string str);
		void	broadcastChannel(std::string str, int fd);

		bool	isOnChannel(std::string user) const;
		bool	isOp(std::string user) const;

		void	setTopic(std::string topic, Client &client);

		bool	getT() const;
		void	setT(bool b);
		timeval	getCreationTime() const;

		void	setOnlyInvite(bool b);

		void	setPass(std::string pass);

		void	setOp(std::string name);
		void	removeOp(std::string name);

		void	setMaxUser(int i);

		bool	getBot() const;
		void	setBot(bool b);
};

#endif