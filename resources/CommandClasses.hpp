#ifndef COMMANDCLASSES_HPP
# define COMMANDCLASSE_HPP

#include "Command.hpp"

class PrivMsgCommand : public Command
{
	PrivMsgCommand(Server *server) {};
	~PrivMsgCommand() {};

	void execute(User* user, std::vector<std::string> args);
};

// a class for every command


#endif