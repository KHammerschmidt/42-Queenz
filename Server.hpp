#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <netinet/in.h>
#include <poll.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

#include <arpa/inet.h>

#include "User.hpp"
#include "Channel.hpp"
#include "Log.hpp"

//weechat braucht keinen header (funktioniert auch so)

class User;
class Channel;
class Log;

class Server
{

public:
	Log		log;
	
private:

	int				_port;
	int				_socket;
	int				_timeout;
	bool			_serverRunningStatus;
	std::string 	_password;
	std::string		_hostname;
	int				_error;

	std::vector<pollfd> 				_pollfds;
	std::vector<pollfd>::iterator 		_pfds_iterator;
	std::map<int, User*> 				_users;
	std::map<std::string, Channel*>		_channels;

public:
	/* Constructor / destructor */
	Server(char** argv);
	~Server();

	/* Set member types and helper functions for input handling */
	int newSocket(void);
	void setSocketSettings(void);
	void setPort(std::string port_str);
	void setServerStatus(bool status);

	/* Get other class members: User, Channel, Pollfds */
	// std::vector<User*> getUsers() const;
	void registerNewUser();
	void printUser();

	/* Functions to run / stop / end the server */
	void run();
	void serverError(int code);

	/* Getters of private member types*/		// --> which ones are needed
	int getSocket() const;
	int getPort() const;
	std::string getPassword() const;
	bool getStatus() const;
	int getTimeout() const;
	std::string getHostname() const;
	bool getServerStatus() const;










/* NOT IMPLEMENTED YET

private:
	time_t				_last_ping;
	int					_maxMembers;

public:
	// tba User* getUser(const std::string name) const;	//nickname or username?
	// tba void deleteUser();

	// tba bool isChannel(std::string const& name);
	// tba Channel& getChannel(std::string name) const;
	// tba void deleteChannel(Channel channel);
	// tba Channel* createChannel(const std::string& name, const std::string& password, const User* user);
	// tba std::vector<Channel *> getChannels() const;

	// tba std::vector<pollfd> getPollfds() const;
 */

};

#endif

































	// other useful functions?
	// void onClientDisconnect(int fd);
	// void onClientConnect();
	// void onClientMessage(int fd);

	// std::string readMessage(int fd);
	// Config &getConfig();
	// std::string getUpTime();

	// void displayUsers();
	// void displayChannels();