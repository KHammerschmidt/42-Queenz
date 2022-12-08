#ifndef USER_HPP
#define USER_HPP

#include <arpa/inet.h>
#include "Server.hpp"
#include "Channel.hpp"
#include "Log.hpp"
#include <sstream>
#include <ctime>

# define BUFFER_SIZE 512
# define MSG_END "\r\n"

// enum USER_STATUS { CONNECTED, PASSWORD, REGISTERED, ONLINE, DELETE}

class User
{
	Log							log;
private:
	int							_fd;
	int							_port;
	time_t						_last_ping;
	std::vector<std::string> 	_dataToSend;


	// std::string		_msg;
	std::string		_buffer;

	std::string 		_username;
	std::string 		_nickname;
	std::string			_fullname;

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
	User(int fd, uint16_t port);
	~User();

	std::string getNickname() const;
	std::string getUsername();
	std::string getFullname() const;
	time_t		getLastPing() const;

	void setNickname();
	void setUsername();
	void setFullname();
	void setLastPing(time_t last_ping);

	std::string setNickUserHost();

	bool getState();
	void receiveData();
	void registerNewUser();
	int getFd();

	void leave() {};
	void write(std::string msg);



};	
// User vector<pollfd, User*>
// Channel <std::string name, User*>




#endif