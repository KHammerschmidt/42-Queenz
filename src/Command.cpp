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
		return 0;												//REGISTER NICKNAME
	else if (message.find("/USER") != std::string::npos)
		return 1;												//REGISTER USERNAME
	else if (message.find("/PING") != std::string::npos)
		return 2;												//SEND PONG
	else if (message.find("/JOIN") != std::string::npos)
		return 3;												//JOIN CHANNEL
	else if (message.find("/PRIVMSG #") != std::string::npos)
		return 4;
	else if (message.find("/PRIVMSG") != std::string::npos || message.find("/NOTICE") != std::string::npos)
		return 5;
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

void Command::sendPrivMsgUser(User* user, std::string msg)
{


	
	int index_of_first_space;

	index_of_first_space = msg.find_first_of(" ");
	std::string command = msg.substr(1, index_of_first_space - 1);
	std::string command_arg = msg.substr(index_of_first_space + 1, msg.length() - index_of_first_space);
	if (command.compare("PRIVMSG") != 0)
		std::cout << "error";



	//find  first space to have lenght of nick
	index_of_first_space = msg.find_first_of(" ");
	if (!index_of_first_space)	
		return ;
	std::string nick_receiver = msg.substr(0, index_of_first_space - 1);
	//check that nick is valid, vector with all nicks?
	/*-> implement ...*/

	//text to print
	std::string text = msg.substr(index_of_first_space + 1, msg.length() - index_of_first_space);

	//? how do I check thatprinted by dest KATHY
	std::cout << user->getNickname() << " | " << text << std::endl;





	Log::printStringCol(CRITICAL, msg);
};

// void 	Command::nick(User* user, const std::string& msg)
// {
// 		if (msg.length() == 0)
// 			return ;

// 		user->setNickname(msg);

// }
