#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include <netinet/in.h>
#include <poll.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <arpa/inet.h>

// class User;
// class Channel;
// namespace irc
// {

class Server
{

private:
	int						port;
	std::string 			password;

	std::vector<pollfd> 	pollfds;

	// std::string 	name;
	// std::string 	admin_name;
	// std::string 	admin_pw;
	// bool			op_rights;

public:
	// std::map<int, User*> user_db;				//	(name, pw, nickname)
	// std::map<std::string, Channel*> channel_db;	// members, ops, name,

public:
	Server(const std::string& port, const std::string& pw);
	~Server();

	void	init_server();
	int		set_port(const std::string& port_str);
	void 	set_password(const std::string& password);
	void 	set_up_socket(void);

};

// } // end namespace ft
#endif