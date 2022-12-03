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

#define _XOPEN_SOURCE_EXTENDED 1
// special behaviour for C++ (use feature to test macro)
// to use ip_mreq_structure use: #define _XOPEN_SOURCE 500 or #define _OPEN_SYS_SOCK_EXT3

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