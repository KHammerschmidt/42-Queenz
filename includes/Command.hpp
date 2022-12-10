#ifndef COMMAND_HPP
# define COMMAND_HPP

#include <string>
#include <iostream>

#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Log.hpp"

enum CommandState{ NICK, USER, PING, JOIN, PRIVMSG_CH, PRIVMSG_U };

// class User;

class Command
{
	private:
		Server* _server;

	public:
		Command(Server* server);
		~Command();

		void invokeMessage(User* user);

		void execute(User* user, const std::string& msg);
		void execute_command(User* user, std::vector<std::string> cmds_to_exec);
		int extract_command(const std::string& msg);

		void print_cmds_to_exec(std::vector<std::string> cmds_to_exec);

		// void sendPrivMsgChannel(User *user, const std::string& msg);
		// void sendPrivMsgUser(User* user, const std::string& msg);
		// void sendPing(User* user, const std::string& msg);

		void sendNickname(User* user, std::string message);
		void sendUsername(User* user, std::string message);
		void sendPong(User* user, std::string message);
		void sendJoin(User* user, std::string message);
		void sendPrivMsgChannel(User* user, std::string message);
		void sendPrivMsgUser(User* user, std::string message);

};

#endif


// invokeMessage()
// 1.

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