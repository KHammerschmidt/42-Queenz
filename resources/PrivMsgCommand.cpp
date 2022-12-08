#include "../includes/CommandCLasses.hpp"

PrivMsgCommand::PrivMsgCommand(Server* server) : Command(server) {};
PrivMsgCommand::~PrivMsgCommand() {};

void PrivMsgCommand::execute(User* user, std::vector<std::string> args)
{
	std::cout << "Here execute PrivMsgCommand" << std::endl;
}

