#ifndef USER_HPP
#define USER_HPP

#include <arpa/inet.h>
#include "Log.hpp"
#include <sstream>
#include <ctime>
#include "Server.hpp"
#include "Channel.hpp"
#include "Command.hpp"
#include <sys/socket.h>

# define BUFFER_SIZE 510
# define MSG_END "\n"	// "\r\n"

enum USER_STATE { CONNECTED, NICKNAME, USERNAME, PASSWORD, REGISTERED, ONLINE, DELETE};
// enum CommandState{ NICK, USER, PING, JOIN, PRIVMSG_CH, PRIVMSG_U };


class Command;

class User
{

private:
	/* server related vars */
	Server*					_server;
	std::string				_hostname;
	std::string				_hostaddr;
	int						_port;

	/* user connection related vars */
	struct sockaddr_in		_user_address;
	int						_fd;
	time_t					_last_ping;
	int						_state;

	/* user identifier vars */
	std::string 			_username;
	std::string 			_nickname;
	std::string				_fullname;
	std::string				_nick_user_host;

	/* command related vars */
	std::string							_buffer;
	std::vector<std::string> 			_dataToSend;
	std::map<std::string, Channel *> 	channels;

	//map with string and command function that takes command object as parameter
	// std::map<std::string, void (*)(Command *)> command_function;
	std::vector<Command *> command_function;

public:
	User(int fd, sockaddr_in u_address, Server* server);
	// User(int fd, uint16_t port);
	~User();

	void onUser();
	void receiveMessage();
	void splitMessage(void);

	void handleMessage();
	void invokeMessage();
	int	 extract_command(const std::string& message);

	void write(const std::string& message) const;
	void reply(std::string& reply);

	void authNickname(void);
	void authUsername(void);
	void authPassword(void);

	std::string getNickname();
	std::string getUsername();
	std::string getFullname() const;
	time_t		getLastPing() const;
	// std::string getPrefix() const;
	// int			getFd() const;
	void setState(int new_state);


	bool isRegistered() const;

	void setNickname(std::string);
	void setUsername();
	void setFullname();
	void setNickUserHost();
	void setLastPing(time_t last_ping);

	int getState();
	void registerNewUser();
	int getFd();


	void welcome() {};
	void join(Channel* channel);
	void leave() {};

	void sendPong();

};

#endif