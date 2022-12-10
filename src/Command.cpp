#include "../includes/Command.hpp"

Command::Command(Server* server) : _server(server) {}
Command::~Command() {}

void Command::invokeMessage(User *user)
{
	std::vector<std::string> cmds_to_exec;

	size_t pos_end_command = user->buffer.find(MSG_END);
	if (pos_end_command == std::string::npos)											//no full command
		return ;

	// while (pos_end_command != std::string::npos)
	// {
	std::string tmp = user->buffer.substr(0, pos_end_command);						//create a substring until delimiter ("\r\n")
	std::cout << " This is the substring of the command: " << tmp << std::endl;		//test printing
	cmds_to_exec.push_back(tmp);													// push at end of vector
	user->buffer.erase(0, user->buffer.length());

	// if (pos_end_command == user->buffer.length())
	// 	break ;
	// tmp = user->buffer.substr(pos_end_command, user->buffer.length());		//cut out first command
	// if (tmp.length() == 0)
	// 	break;
	// else
	// 	pos_end_command = user->buffer.find(MSG_END);									//search for an additional command ("\r\n") would be present
	// }
	// print_cmds_to_exec(cmds_to_exec);												//for debug
	execute_command(user, cmds_to_exec);
}

void Command::print_cmds_to_exec(std::vector<std::string> cmds_to_exec)
{
	for (std::vector<std::string>::iterator iter = cmds_to_exec.begin(); iter != cmds_to_exec.end(); iter++)
		Log::printStringCol(WARNING, *iter);
}

void Command::execute_command(User* user, std::vector<std::string> cmds_to_exec)
{
	for (std::vector<std::string>::iterator iter = cmds_to_exec.begin(); iter != cmds_to_exec.end(); iter++)
	{
		switch(extract_command(*iter))	//*iter is a string
		{
			case NICK:
				sendNickname(user, *iter);
				break ;
			case USER:
				sendUsername(user, *iter);
				break ;
			case PING:
				sendPong(user, *iter);
				break ;
			case JOIN:
				sendJoin(user, *iter);
				break ;
			case PRIVMSG_CH:
				sendPrivMsgChannel(user, *iter);
				break ;
			case PRIVMSG_U:
				sendPrivMsgUser(user, *iter);
				break ;
			default:
				Log::printStringCol(CRITICAL, "ERROR: COMMAND UNKNOWN");
				user->buffer.erase(0, user->buffer.length());
				return ;
		}
	}
}

int Command::extract_command(const std::string& message)
{
	if (message.find("/NICK") != std::string::npos)
		return 1;												//REGISTER NICKNAME
	else if (message.find("/USER") != std::string::npos)
		return 2;												//REGISTER USERNAME
	else if (message.find("/PING") != std::string::npos)
		return 3;												//SEND PONG
	else if (message.find("/JOIN") != std::string::npos)
		return 4;												//JOIN CHANNEL
	else if (message.find("/PRIVMSG #") != std::string::npos)
		return 5;												//PRIVMSG CHANNEL
	else if (message.find("/PRIVMSG") != std::string::npos || message.find("/NOTICE") != std::string::npos)
		return 6;												//PRIVMSG
	else
		return -1;
}


void Command::sendNickname(User* user, std::string message)
{
	if (!user || message.length() == 0)
		return ;

	Log::printStringCol(CRITICAL, message);
};

void Command::sendUsername(User* user, std::string message)
{
	if (!user || message.length() == 0)
		return ;
	Log::printStringCol(CRITICAL, message);
};

void Command::sendPong(User* user, std::string message)
{
	if (!user || message.length() == 0)
		return ;
	Log::printStringCol(CRITICAL, message);
};

void Command::sendJoin(User* user, std::string message)
{
	if (!user || message.length() == 0)
		return ;
	Log::printStringCol(CRITICAL, message);
};

void Command::sendPrivMsgChannel(User* user, std::string message)
{
	if (!user || message.length() == 0)
		return ;
	Log::printStringCol(CRITICAL, message);
};

void Command::sendPrivMsgUser(User* user, std::string message)
{
	if (!user || message.length() == 0)
		return ;
	Log::printStringCol(CRITICAL, message);
};
