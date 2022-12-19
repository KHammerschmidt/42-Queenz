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
		bool 					authentified;

		/* command related vars */
		std::string							_buffer;
		std::vector<std::string> 			_dataToSend;
		// std::map<std::string, Channel *> 	channels; -> implemented in server
		std::vector<Command *> 				command_function;

		/*user state in channel*/
		std::string				_userStatusInChannel;
	public:
		bool					_first_nick;//make private and getter

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
		std::string getUserChannelStatus();
		// std::string getPrefix() const;
		// int			getFd() const;
		void setState(int new_state);
		bool isRegistered() const;

		void setNickname(const std::string& nickname);
		void setUsername(const std::string& username);
		void setFullname(std::string fullname);
		void setPassword(std::string pw);
	  	void setNickUserHost();
		void setLastPing(time_t last_ping);
		void setUserChannelStatus(const std::string &status);

		int getState();
		void registerNewUser();
		int getFd();

		void welcome() {};
		void join(Channel* channel);
		void leave() {};

		void sendPong();

		void setAuth(int num);
		int getAuth() const;
		void authenticate_user(void);


};

#endif


// switch (poll(fds, _users.size(), SERVER_TIMEOUT))
// {
//     case -1:
//         irc_log(error, "poll error: ", "");
//         break ;
//     case 0:
//         irc_log(error, "poll timeout: ", "");
//         break ;
//     default:
// 	{
//         accept_connections();
//         for (Users::iterator user_it = _users.begin(); user_it < _users.end(); user_it++)
//         {
//             ++user_it->socket.revents;
//             if (user_it == _users.begin() || user_it->socket.revents == 0)                  // we don't want to get input from server and recently accepted users before polling them
//                 continue ;
//             char buffer[BUFFER_SIZE];
//             std::memset(&buffer, 0, sizeof(buffer));
//             int err = recv(user_it->socket.fd, &buffer, sizeof(buffer), 0);                 // err < 0 -> nothing to receive!
//             if (err == 0)
//             {                                                                 // rc == 0 -> connection closed by User
//                 irc_log(trace, "closing user: ", user_it->socket.fd);
//                 user_it->part_all_channels(_channels);
//                 close(user_it->socket.fd);
//                 _users.erase(user_it);
//             }
//             else
//             {
//                 parse_cmd(user_it, buffer);
//             }
//         } /* End of existing connection is readable */
// 	}
// } /* End of switch */