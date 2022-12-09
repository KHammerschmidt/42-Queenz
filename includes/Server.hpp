#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <errno.h>				// FOR DEBUG ONLY!!


#include <ctime>
#include <netinet/in.h>
#include <poll.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>
#include <arpa/inet.h>

#include "User.hpp"
#include "Channel.hpp"
#include "Log.hpp"

#define HOSTNAME "@42-Queenz.fr.42"
#define BUFFER_SIZE 520

class User;
class Channel;

class Server
{

	private:
		int								_port;						//int oder std::string
		int								_sockfd;
		int								_timeout;
		bool							_serverRunningStatus;
		// time_t							_last_ping;
		std::string 					_password;

		std::vector<pollfd> 			_pollfds;
		std::map<int, User*>			_users;						// std::pair<pollfd, User*> _user;
		std::map<std::string, Channel*> _channels;

	public:
		std::vector<pollfd>::iterator 	pfds_iterator;
		std::map<int, User*>::iterator 	user_iterator;
		std::map<std::string, Channel*>	channel_iterator;

	private:
		void newSocket(void);
		void sendPing();			//who sends the ping? client? automatic?

		void connectNewUser();
		void serverError(int code);

		void disconnectUser(User* user);
		void deleteUser(User* user);
		void printUser();

	public:
		Server(char** argv);
		~Server();

		void run();

		int 		getPort() const;
		int 		getTimeout() const;
		bool 		getServerStatus() const;
		std::string getPassword() const;
		User* 		getUser(int fd);

		void setPort(std::string port_str);
		void setServerStatus(bool status);
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