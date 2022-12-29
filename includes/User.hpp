#ifndef USER_HPP
#define USER_HPP

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sstream>
#include <ctime>
#include <stdlib.h>

#include "Log.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Command.hpp"

# define BUFFER_SIZE 512
# define MSG_END "\r\n"

enum USER_STATE { CONNECTED, REGISTERED, ONLINE, DELETE };

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
		std::string				_nickOP;
		std::string				_password;

		/* command related vars */
		std::string					_buffer;
		std::vector<std::string> 	_dataToSend;
		std::vector<Command *> 		command_function;

		/*user state in channel*/
		std::string				_userStatusInChannel;

	public:
		bool					_first_nick;			//make private and getter

		User(int fd, sockaddr_in u_address, Server* server);
		~User();

		void onUser();
		void receive();
		void split();
		void invoke();
		void write();
		void reply();
		void clearCommandFunction(void);

		int	getFd() const;
		int getState() const;
		std::string getNicknameOP() const;
		std::string getNickname() const;
		std::string getUsername() const;
		std::string getFullname() const;
		std::string getPassword() const;
		std::string getNickUserHost() const;
		std::string getUserChannelStatus() const;

	  	void setNickUserHost();
		void setNicknameOP();
		void setState(int new_state);
		void setPassword(std::string pw);
		void setLastPing(time_t last_ping);
		void setFullname(std::string fullname);
		void setNickname(const std::string& nickname);
		void setUsername(const std::string& username);
		void setUserChannelStatus(const std::string &status);

		bool isRegistered() const;

		void leave() {};

};

#endif