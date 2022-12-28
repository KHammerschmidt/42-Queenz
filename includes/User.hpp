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
# define MSG_END "\r\n"			// "\r\n"

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
		int						_state;

		/* user identifier vars */
		std::string 			_username;
		std::string 			_nickname;
		std::string				_fullname;
		std::string				_nick_user_host;
		std::string				_password;

		/* command related vars */
		std::string							_buffer;
		std::vector<std::string> 			_dataToSend;
		std::vector<Command *> 				command_function;

		/*user state in channel*/
		std::string				_userStatusInChannel;

	public:
		bool					_first_nick;//make private and getter

		// std::map<std::string, Channel *> 	channels; -> implemented in server

		User(int fd, sockaddr_in u_address, Server* server);
		~User();

		void onUser();
		void receive();
		void split(void);
		void invoke();
		void write();
		void reply();
		void clearCommandFunction(void);

		void registerNewUser();
		bool isRegistered();

		int	getFd() const;
		int getState() const;
		std::string getNickname() const;
		std::string getUsername() const;
		std::string getFullname() const;
		std::string getPassword() const;
		std::string getNickUserHost() const;
		std::string getUserChannelStatus() const;

	  	void setNickUserHost();
		void setState(int new_state);
		void setPassword(std::string pw);
		void setLastPing(time_t last_ping);
		void setFullname(std::string fullname);
		void setNickname(const std::string& nickname);
		void setUsername(const std::string& username);
		void setUserChannelStatus(const std::string &status);

		void welcome() {};
		void join(Channel* channel);
		void leave() {};

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
/*
#include <csignal>
#include <iostream>
#include <cstdlib>
 
class Tester {
public:
    Tester()  { std::cout << "Tester ctor\n"; }
    ~Tester() { std::cout << "Tester dtor\n"; }
};
 
Tester static_tester; // Destructor not called
 
void signal_handler(int signal) 
{
    if (signal == SIGABRT) {
        std::cerr << "SIGABRT received\n";
    } else {
        std::cerr << "Unexpected signal " << signal << " received\n";
    }
    std::_Exit(EXIT_FAILURE);
}
 
int main()
{
    Tester automatic_tester; // Destructor not called
 
    // Setup handler
    auto previous_handler = std::signal(SIGABRT, signal_handler);
    if (previous_handler == SIG_ERR) {
        std::cerr << "Setup failed\n";
        return EXIT_FAILURE;
    }
 
    std::abort();  // Raise SIGABRT
    std::cout << "This code is unreachable\n";
}*/

