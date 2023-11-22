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
		std::string					_password;
		unsigned int				_maxUser;
		unsigned int				_currentUser;
		bool						_needpass;
		bool						_onlyinvite;

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
		std::string	getName(void) const;

		unsigned int	getMaxUser(void) const;
		unsigned int	getCurrentUser(void) const;

		bool	getNeedPass(void) const;
		bool	getOnlyInvite(void) const;

		bool	checkPass(std::string pass) const;
		bool	isInvited(std::string user) const;

		void	addInvited(std::string username);
		void	addMember(Client &client);

		void	removeMember(Client &client);

		void	broadcast(std::string str);
};

#endif