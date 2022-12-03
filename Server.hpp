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

#define _XOPEN_SOURCE_EXTENDED 1
// special behaviour for C++ (use feature to test macro)
// to use ip_mreq_structure use: #define _XOPEN_SOURCE 500 or #define _OPEN_SYS_SOCK_EXT3

// class User;
// class Channel;

// namespace irc
// {

class User;
class Channel;

class Server
{

private:
	bool					_running;
	int						_port;
	std::string 			_password;
	int						_socket;
	int						_timeout;
	std::string				_hostname;

	std::vector<pollfd> 	_pollfds;			//pollfds iterator?
	std::map<int, User*> 	_users;				// map<user-ID/user-fd, user object>
	std::map<std::string, Channel*> _channels;


	// struct sockaddr_in serv_address; // struct in class?
	// server operator: std::string name; std::string admin_name; std::string admin_pw; bool op_rights;

public:
	typedef std::vector<pollfd>::iterator pollfds_iterator;

public:
	Server(char** argv);
	~Server();

	/* Getters of private member types*/
	bool getStatus() const { return this->_running; }
	int getPort() const { return this->_port; }
	std::string& getPassword() const { return this->_password; }
	int getSocket() const { return this->_socket; }
	int getTimeout() const { return this->_timeout; }
	std::string& getHostname() const { return this->_hostname; }

	std::vector<User*> getUsers() const;
	std::map<std::string, Channel*> getChannels() const;
	std::vector<pollfd> getPollfds() const;


	/* Setters of private member types */
	void setStatus(bool status);
	int set_port(const std::string& port_str);
	const std::string& set_password(const std::string& pw);
	int new_socket(void);
	std::string setHostname();

	//void addUser();
	//void acceptUser();
	//void deleteUser();
	void printUser();



	void run();


};

// } // end namespace ft
#endif