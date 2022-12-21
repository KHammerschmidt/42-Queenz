#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <ctime>
#include <netinet/in.h>
#include <poll.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>
#include <arpa/inet.h>
#include <stdlib.h>

#include "User.hpp"
#include "Channel.hpp"
#include "Log.hpp"

#define HOSTNAME "42-Queenz.fr.42"

class User;
class Channel;

class Server
{

	private:
		uint16_t						_port;
		int								_sockfd;
		int								_timeout;
		bool							_serverRunningStatus;
		std::string 					_password;
		struct sockaddr_in				_serv_address;

		std::vector<pollfd> 			_pollfds;

		int			authentified;
		
		// time_t							_last_ping;

	public:
		std::vector<pollfd>::iterator 	pfds_iterator;
		std::map<int, User*>::iterator 	user_iterator;
		std::map<std::string, Channel*>	channel_iterator;

	//RUS-> ich habe _channels vector und  _channel_users map implementiert, die muessen hier deleted werde.

		std::map<int, User*>			_users;

		std::vector<User*> getUsers() const; //1) vector all users
		std::vector<Channel*>	_channels; //2) vector all channels
		std::multimap<std::string, User*> 	_channel_users;//3) mappair channel/user, with multimap can use key channel multiple times
		std::vector<std::string>			_channels_by_name;//->cancel, not needed 

	private:
		void setNewSocket(void);
		void sendPing();

		void connectNewUser();
		void serverError(int code);

		void disconnectUser(User* user);
		void deleteUser(User* user);
		void deleteChannel(Channel* channel);
		void printUser();

	public:
		Server(char** argv);
		~Server();

		void run();

		sockaddr_in	getAddr() const { return this->_serv_address; }
		int 		getPort() const;
		int 		getTimeout() const;
		bool 		getServerStatus() const;
		std::string getPassword() const;
		User* 		getUser(int fd);
		std::string getNickUserHost() const;

		void setPort(std::string port_str);
		void setServerStatus(bool status);
		void setFullname(std::string fullname);

		void setAuth(int num);
		int getAuth() const;
		int getServerFd(void);

};

#endif
