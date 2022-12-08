#ifndef COMMAND_HPP
# define COMMAND_HPP

#include <string>
#include <iostream>

#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"

enum CommandState{ PRIVMSGCH, PRIVMSGU, PING};

class Command
{
	private:
		Server* _server;

	public:
		Command(Server* server, User* user, const std::string& msg);
		virtual ~Command();

		void execute(User* user, const std::string& msg);
		int extractCommand(const std::string& msg);

		void sendPrivMsgChannel(User *user, const std::string& msg);
		void sendPrivMsgUser(User* user, const std::string& msg);
		void sendPing(User* user, const std::string& msg);

};

#endif


// maybe a vector with command args  ????
// vector[0] == command schlagwort 
// vector[1] == user
// vector[2] == user to receive
// vector[3] = channel name

// functions or map with commands?
// std::map<std::string, Command*> _commands;
	// this->_commands["PRIVMSG"] = new PrivMsgCommand(_server, user, msg);
	// this->_commands["PRIVMSG #"] = new PrivMsgChannelCommand(_server, user, msg);

	// this->_commands["NICK"] = new NickCommand(_server, user, msg, false);
	// this->_commands["USER"] = new UserCommand(_server, user, msg, false);
	// this->_commands["QUIT"] = new QuitCommand(_server, user, msg, false);
	// this->_commands["PING"] = new PingCommand(_server, user, msg);

	//  ......... tbc


// 	Command::~Command()
// {
// 	for (std::map<std::string, Channel*>::iterator iter = _commands.begin(); iter != _commands.end(); iter++)
// 		delete iter->second;
// }