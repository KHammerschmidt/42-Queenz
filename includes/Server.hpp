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
#define BUFFER_SIZE 510

class User;
class Channel;

//static Server pointer
// gucken ob server initialisiert ist, ansonsten neue Instanz vom Constructor

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
		std::map<int, User*>			_users;
		std::map<std::string, Channel*> _channels;


		// time_t							_last_ping;

	public:
		std::vector<pollfd>::iterator 	pfds_iterator;
		std::map<int, User*>::iterator 	user_iterator;
		std::map<std::string, Channel*>	channel_iterator;

		//need to set private after test phase, otherwise is inacessible in command::fin_user_in_server
		// std::map<int, User*>			_users;

		std::vector<User*> getUsers() const;

	private:
		void newSocket(void);
		void sendPing();			//who sends the ping? client? automatic?

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
};

#endif




/* NOT IMPLEMENTED YET | needed?

--> USER
	// tba User* getUser(const std::string name) const;	//nickname or username?
	// tba void deleteUser();

--> CHANNEL
	// tba bool isChannel(std::string const& name);
	// tba Channel& getChannel(std::string name) const;
	// tba void deleteChannel(Channel channel);
	// tba Channel* createChannel(const std::string& name, const std::string& password, const User* user);
	// tba std::vector<Channel *> getChannels() const;
 */

































	// other useful functions?
	// void onClientDisconnect(int fd);
	// void onClientConnect();
	// void onClientMessage(int fd);

	// std::string readMessage(int fd);
	// Config &getConfig();
	// std::string getUpTime();

	// void displayUsers();
	// void displayChannels();