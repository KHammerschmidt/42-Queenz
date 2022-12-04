#ifndef USER_HPP
#define USER_HPP

#include "Server.hpp"
#include "Channel.hpp"

#include <sstream>

class User
{

private:
	std::string 		_username;
	std::string 		_nickname;
	std::string			_fullname;
	std::string 		_hostname;
	int	_fd;				//every user has their own fd
	int					_port;
	// std::string 		_hostaddr;
	std::string			_nick_user_host;
	bool				_state;

	std::vector<pollfd>::iterator	pollfds_iterator;

	std::string buffer;		//command Zwischenspeicher
	std::map<int, Channel *> channels;

	// enum {channel_creator, channel_operator, channel_member};

	// std::map<Channel&, rights>			channelRights;		//list of rights connected to user and channel
	// std::map<std::string, Channel*>		channel_lst;		//list of channels that user is member of
	// std::string _channel_name: 		//name + @ (don't know which name)


public:
	User(int fd, int port);
	~User();

	std::string getNickname() const;
	std::string getUsername();
	std::string getFullname() const;

	void setNickname();
	void setUsername();
	void setFullname();

	std::string setNickUserHost();

	bool getState();
	void receive();
	void registerNewUser();
};

#endif