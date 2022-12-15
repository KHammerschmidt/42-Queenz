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

# define BUFFER_SIZE 510
# define MSG_END "\n"			// "\r\n"

// enum USER_STATE { CONNTECTED = 0, REGISTERD = 3, ONLINE = 4, DELETE = 5 };
enum USER_STATE { CONNECTED, NICKNAME, USERNAME, PASSWORD, REGISTERED, ONLINE, DELETE};

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
		std::string 			_username;		//usually set automatically by client
		std::string 			_nickname;
		std::string				_fullname;
		std::string				_nick_user_host;
		std::string				_password;

		/* command related vars */
		std::string							_buffer;
		std::vector<std::string> 			_dataToSend;
		// std::map<std::string, Channel *> 	channels;
		std::vector<Command *> 				command_function;
		int authentified;

	public:
		User(int fd, sockaddr_in u_address, Server* server);
		~User();

		void onUser();
		void receive();
		void split(void);
		void invoke();
		void write();
		void reply();

		int	 extract_command(const std::string& message);
		void clearCommandFunction(void);

		void write(const std::string& message) const;

		void authNickname(void);
		void authUsername(void);
		void authPassword(void);

		std::string getNickname();
		std::string getUsername();
		std::string getFullname() const;
		time_t		getLastPing() const;
		std::string getNickUserHost() const;
		std::string getPassword() const;
		// std::string getPrefix() const;
		// int			getFd() const;
		void setState(int new_state);
		bool isRegistered() const;

		void setNickname(const std::string& nickname);
		void setUsername(const std::string& username);
		void setFullname(std::string fullname);
		void setPassword(std::string pw);
	  	void setNickUserHost(std::string str);
		void setLastPing(time_t last_ping);
   		void setNickUserHost2(std::string);	// --> kannst auch gerne die setNickUserHost() benutzen :)

		int getState();
		void registerNewUser();
		int getFd();

		void welcome() {};
		void join(Channel* channel);
		void leave() {};

		void sendPong();

		void setAuth(int num);
		int getAuth() const;


};

#endif