#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <map>
#include <vector>
#include <string>

class User;
class Server;

class Channel
{

private:
	std::string _name;
	std::string	_topic;


	int	_user_status; 

	std::string mode;

public:
	Channel();
	Channel(const std::string name);
	~Channel();

	void 		setName(const std::string name);
	void		setUserStatus(int status);
	std::string getName() const;
	int			getUserStatus();

	std::map<int, User*>  getChannelMembers();
	std::map<int, User*>  getChannelOperators();
	std::map<int, User*>  getChannelCreator();


	public:
		std::vector<User*> _channel_members; 	
		std::vector<User*> _channel_operators; 


	void		addUser(User *user);
	void		deleteUser(User *user);

	void giveTakeOpPrivileges(User *user_not_op, User *user_op, std::string mode);
};

#endif



