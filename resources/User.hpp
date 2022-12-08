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

// enum USER_STATE { CONNECTED, NICK, PASSWORD, REGISTERED, ONLINE, DELETE}
// enum USER_ROLE {CREATOR, OPERATOR, USER}

class User
{

private:
	int						_fd;
	int						_port;
	time_t					_last_ping;
	bool					_state;			// spaeter enum

	std::string 			_username;
	std::string 			_nickname;
	std::string				_fullname;
	std::string				_nick_user_host;

	std::string							_buffer;
	std::vector<std::string> 			_dataToSend;
	std::map<std::string, Channel *> 	channels;
	// Channel* channel;

public:
	User(int fd, uint16_t port);
	~User();

	std::string getNickname() const;
	std::string getUsername()const ;
	std::string getFullname() const;
	time_t		getLastPing() const;
	// std::string getPrefix() const;
	// int			getFd() const;

	bool isRegistered() const;

	void setNickname(std::string);
	void setUsername();
	void setFullname();
	void setNickUserHost();
	void setLastPing(time_t last_ping);

	bool getState();
	int getFd();
	
	void receiveData();
	void registerNewUser();

	void write(std::string msg);
	void reply(std::string& reply) {};

	void welcome() {};
	void join(Channel* channel) {};
	void leave() {};
	
};	

#endif