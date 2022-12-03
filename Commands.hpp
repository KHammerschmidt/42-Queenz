#ifndef COMMANDS_HPP
# define COMMANDS_HPP

#include <string>
#include <iostream>
#include "Server.hpp"
#include "User.hpp"

enum {PRIVMSG_PRIV, PRIVMSG_CHANNEL, REGISTER}

// namespace irc
// {

class Commands
{

public:
	Commands();
	~Commands();

	void getCommand(const std::string& cmd);
	int getCommandID(const std::string& cmd);



	void send_privmsg_priv();
	void send_privmsg_channel();
	void registe_new_user();
	void invalid_cmd();

};


// } //end namespace irc


#endif
