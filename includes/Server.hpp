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

#include "Log.hpp"
#include "User.hpp"
#include "Channel.hpp"

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

		//RUS-> ich habe _channels vector und  _channel_users map implementiert, die muessen hier deleted werde.
		std::map<int, User*>				_users;

	public:
		std::vector<Channel*>				_channels; 			//2) vector all channels
		std::multimap<std::string, User*> 	_channel_users;		//3) mappair channel/user, with multimap can use key channel multiple times

		// std::vector<std::string>			_channels_by_name;	//->cancel, not needed

	public:
		std::vector<pollfd>::iterator 	pfds_iterator;
		std::map<int, User*>::iterator 	user_iterator;
		std::vector<Channel*>::iterator	channel_iterator;

	private:
		void connectNewUser();
		void disconnectUser(int fd, bool state);
		void serverError(int code);
		void deleteUser(int fd);
		void deleteChannel(Channel* channel);

		void setNewSocket(void);
		void setPort(std::string port_str);

	public:
		Server(char** argv);
		~Server();

		void run();

		int 		getServerFd(void);
		int 		getPort() const;
		int 		getTimeout() const;
		bool 		getServerStatus() const;
		User* 		getUser(int fd);
		std::string getPassword() const;
		sockaddr_in	getAddr() const;

		//RUSLAN
		std::vector<User*> getUsers() const;

		void setServerStatus(bool status);
};

#endif
