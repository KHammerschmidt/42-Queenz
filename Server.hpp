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

#define _XOPEN_SOURCE_EXTENDED 1
// special behaviour for C++ (use feature to test macro)
// to use ip_mreq_structure use: #define _XOPEN_SOURCE 500 or #define _OPEN_SYS_SOCK_EXT3

// class User;
// class Channel;

class User;
class Channel;
class Log;

class Server
{

private:
	int						_port;
	int						_socket;
	int						_timeout;
	bool					_status;
	std::string 			_password;
	std::string				_hostname;

	std::vector<pollfd> 			_pollfds;			//pollfds iterator?
	std::map<int, User*> 			_users;				// map<user-ID/user-fd, user object>
	std::map<std::string, Channel*>	_channels;

	int						_error;
	time_t					_last_ping;


	// int					_maxMembers;

	void acceptUser();
	void sendPing();

	void displayUsers();
	void displayChannels();


public:
	// typedef std::vector<pollfd>::iterator pollfds_iterator;

public:
	Server(char** argv);
	~Server();

	/* Setters of private member types */
	void setPort(const std::string& port_str);
	void setPassword(const std::string& pw);
	void setHostname();
	void setServerStatus(bool status);

	/* Getters of private member types*/
	int getSocket() const;
	int getPort() const;
	std::string getPassword() const;
	bool getStatus() const;
	int getTimeout() const;
	std::string getHostname() const;

	/* Get other class members: User, Channel, Pollfds */
	User* getUser(const std::string name) const;	//nickname or username?
	std::vector<User*> getUsers() const;
	void deleteUser();
	// void addUser();
	// bool acceptUser();
	void printUser();

	bool isChanel(std::string const& name);
	Channel& getChannel(std::string name) const;
	void deleteChannel(Channel channel);
	Channel* createChannel(const std::string& name, const std::string& password, const User* user);
	std::vector<Channel *> getChannels() const;

	std::vector<pollfd> getPollfds() const;

	void run();
	int newSocket(void);
	void error(int err_code);


	// void onClientDisconnect(int fd);
	// void onClientConnect();
	// void onClientMessage(int fd);

	// std::string readMessage(int fd);
	// Config &getConfig();
	// std::string getUpTime();
};

#endif












	// struct sockaddr_in serv_address; // struct in class?
	// server operator: std::string name; std::string admin_name; std::string admin_pw; bool op_rights;