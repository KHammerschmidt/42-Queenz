#include "../includes/Command.hpp"
#include <iostream>

bool Command::getCommandState(void) const { return this->_state; }			//command var _state (ready to send/or not)


Command::Command(User* user, Server* server, std::string message)
	: _user(user), _server(server), _query(message), _command_state(false), _reply_state(false)
{
	if (message.find("/NICK") != std::string::npos)
		nick(user, message);
	else if (message.find("/PRIVMSG") != std::string::npos || message.find("/NOTICE") != std::string::npos)
		sendPrivMsgUser(user, message);
	else
	{
		Log::printStringCol(WARNING, "WARNING: NO VALID COMMAND FOUND.");
		this->_valid_command = false;
	}

	// else if (message.find("/USER") != std::string::npos)
	// 	user(user, message);
	// else if (message.find("/PING") != std::string::npos)
	// 	return 2;
	// else if (message.find("/JOIN") != std::string::npos)
	// 	return 3;
	// else if (message.find("/PRIVMSG #") != std::string::npos)
	// 	return 4;

	// size_t pos;
	// std::string delimiter(":");	//before colon: command & user (the one who send message)

	// if ((pos = message.find(delimiter)) != std::string::npos)
	// {
	// 	std::string remainder;
	// 	std::string tmp = message.substr(0, pos);
	// 	message.erase(0, pos + delimiter.length());
	// 	remainder = message;
	// 	message = tmp;
	// }

	// this->_parameters = split(message, " ");
	// prefix = *(this->_parameters.begin());					//nickname (origin of message)
	// for (size_t i = 0; i < prefix.length(); i++)			//prefix in upper case characters
	// 	prefix[i] = std::toupper(prefix[i]);
	// this->_parameters.erase(_parameters.begin());			//delete prefix from vector
}

// void Command::execute(void)
// {
// 	std::cout << "This is Command::execute()" << std::endl;
// 	std::cout << prefix << std::endl;
// }

// std::vector<std::string> Command::split(std::string str, std::string delimiter)
// {
// 	size_t pos;
// 	std::string tmp;
// 	std::vector<std::string> splitted_message;

// 	while ((pos = str.find(delimiter))!= std::string::npos)
// 	{
// 		tmp = str.substr(0, pos);
// 		splitted_message.push_back(tmp);
// 		str.erase(0, pos + delimiter.length());
// 	}
// 	return splitted_message;
// }

// std::string Command::getPrefix() { return this->prefix; }
// std::vector<std::string> Command::getParameters() { return this->_parameters; }
// std::string Command::getQuery() { return this->_query; }

// void Command::setStop(bool new_stop) { stop = new_stop; }
// bool Command::getStop() { return this->stop; }

// void Command::reply(User& user, int code, std::string arg1 = "") {}
// void Command::reply(int code, std::string arg1 = "") {}








// Command::Command(Server* server) : _server(server) {}
// Command::~Command() {}

// void Command::invokeMessage(User *user)
// {
// 	std::vector<std::string> cmds_to_exec;

// 	size_t pos_end_command = user->_buffer.find(MSG_END);
// 	if (pos_end_command == std::string::npos)											//no full command
// 		return ;

// 	// while (pos_end_command != std::string::npos)
// 	// {
// 	std::string tmp = user->_buffer.substr(0, pos_end_command);						//create a substring until delimiter ("\r\n")
// 	cmds_to_exec.push_back(tmp);													// push at end of vector
// 	user->_buffer.erase(0, user->_buffer.length());

// 	// if (pos_end_command == user->_buffer.length())
// 	// 	break ;
// 	// tmp = user->_buffer.substr(pos_end_command, user->_buffer.length());		//cut out first command
// 	// if (tmp.length() == 0)
// 	// 	break;
// 	// else
// 	// 	pos_end_command = user->_buffer.find(MSG_END);									//search for an additional command ("\r\n") would be present
// 	// }
// 	// print_cmds_to_exec(cmds_to_exec);												//for debug
// 	execute_command(user, cmds_to_exec);
// }
// 	void Command::execute_command(User* user, std::vector<std::string> cmds_to_exec)
// void Command::execute_command(User* user, std::vector<std::string> cmds_to_exec)
// {
// 	for (std::vector<std::string>::iterator iter = cmds_to_exec.begin(); iter != cmds_to_exec.end(); iter++)
// 	{
// 		switch(extract_command(*iter))	//*iter is a string
// 		{
// 			case NICK:
// 				sendNickname(user, *iter);
// 				break ;
// 			case USER:
// 				sendUsername(user, *iter);
// 				break ;
// 			case PING:
// 				sendPong(user, *iter);
// 				break ;
// 			case JOIN:
// 				sendJoin(user, *iter);
// 				break ;
// 			case PRIVMSG_CH:
// 				sendPrivMsgChannel(user, *iter);
// 				break ;
// 			case PRIVMSG_U:
// 				sendPrivMsgUser(user, *iter);
// 				break ;
// 			default:
// 				Log::printStringCol(CRITICAL, "ERROR: COMMAND UNKNOWN");
// 				// user->_buffer.erase(0, user->_buffer.length());
// 				return ;
// 		}
// 	}
// }

// int Command::extract_command(const std::string& message)
// {
// 	if (message.find("/NICK") != std::string::npos)
// 		return 0;												//REGISTER NICKNAME
// 	else if (message.find("/USER") != std::string::npos)
// 		return 1;												//REGISTER USERNAME
// 	else if (message.find("/PING") != std::string::npos)
// 		return 2;												//SEND PONG
// 	else if (message.find("/JOIN") != std::string::npos)
// 		return 3;												//JOIN CHANNEL
// 	else if (message.find("/PRIVMSG #") != std::string::npos)
// 		return 4;
// 	else if (message.find("/PRIVMSG") != std::string::npos || message.find("/NOTICE") != std::string::npos)
// 		return 5;
// 	else
// 		return -1;
// }

// void Command::sendNickname(User* user, std::string message)
// {
// 	if (!user || message.length() == 0)
// 		return ;

// 	Log::printStringCol(CRITICAL, message);
// };

// void Command::sendUsername(User* user, std::string message)
// {
// 	if (!user || message.length() == 0)
// 		return ;
// 	Log::printStringCol(CRITICAL, message);
// };

// void Command::sendPong(User* user, std::string message)
// {
// 	if (!user || message.length() == 0)
// 		return ;
// 	Log::printStringCol(CRITICAL, message);
// };

// void Command::sendJoin(User* user, std::string message)
// {
// 	if (!user || message.length() == 0)
// 		return ;
// 	Log::printStringCol(CRITICAL, message);
// };

// void Command::sendPrivMsgChannel(User* user, std::string message)
// {
// 	if (!user || message.length() == 0)
// 		return ;
// 	Log::printStringCol(CRITICAL, message);
// };


/*make compile*/
int	Command::find_user_in_server(const std::string nickname_receiver){
	for (std::map<int, User*>::iterator iter = this->_server->_users.begin(); iter != this->_server->_users.end(); iter++)
	{
		if (iter->second->getNickname() == nickname_receiver)
		{
			this->user_receiver = nickname_receiver;			//in Command classe variable: User* user_receiver;
			return (1);
		}
		else if (iter == this->_server->_users.end())
		{
			std::cout << "INVALID USER REQUEST. USER DOES NOT EXIST" << std::endl;
			return (0);
		}
	}
	return (1);
}





void Command::sendPrivMsgUser(User* user, std::string msg)		//13:57:27 ruslan1 | hi
{																//4:01:04   libera  -- | MSG(ruslan1): hello
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

	//
	if (find_user_in_server(nick_receiver) == 0)
		return ;


//check that nick is valid, vector with all nicks? and that exists.
	/*-> implement ...*/


	this->_command_state = true;
	this->_reply_state = true;
	//text to print
	text = msg.substr(index_of_first_space + 1, msg.length() - index_of_first_space);

	std::string ouput_to_client;
	ouput_to_client.append(":");
	ouput_to_client.append(user->getNickname());
	ouput_to_client.append("!");
	ouput_to_client.append(user->getNickname());
	ouput_to_client.append(HOSTNAME);
	ouput_to_client.append(" ");
	ouput_to_client.append(msg);
	user->setNickUserHost2(ouput_to_client);



	//? how do I check thatprinted by dest KATHY
	//std::cout << user->getNickname() << " | " << text << std::endl;
	//	//-> implement anstatt oben: ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
	Log::printStringCol(CRITICAL, msg);
};


void 	Command::nick(User* user, const std::string& msg)
{
		if (msg.length() == 0)
			return ;

		this->_command_state = true;
		this->_reply_state = true;
		user->setNickname(msg);
}

void Command::sendPrivNoticeUser(User* user, std::string msg)	//same as private message, but doesnt open a query, direct in channel
{																//receiver see: 23:33:09   ircserv  -- | ruslan (~ruslan@ip_addr): hi
																//sender: │14:00:34   libera  -- | Notice -> ruslan1: hi
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

	this->_command_state = true;
	this->_reply_state = true;

	//std::cout << user->getNickname() << " : " << text << std::endl;
	//-> implement anstatt oben: ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

	/*von privatemessage kopiert -> verarbeiten*/
	std::string ouput_to_client;
	ouput_to_client.append(":");
	ouput_to_client.append(user->getNickname());
	ouput_to_client.append("!");
	ouput_to_client.append(user->getNickname());
	ouput_to_client.append(HOSTNAME);
	ouput_to_client.append(" ");
	ouput_to_client.append(msg);
	user->setNickUserHost2(ouput_to_client);



	Log::printStringCol(CRITICAL, msg);
};



//in server add a vector with all channels names or in user all the joined channels; so I can test here if need to create a new one or not, without creating a temp channel to check it(line 29 channel)
// void Command::sendJoin(User* user, const std::string message)
// {
// 	if (!user || message.length() == 0)
// 		return ;

// 	//Channel::createChannel(message);

// 	Log::printStringCol(CRITICAL, message);
// };

// void Command::sendQuit(User* user, const std::string message)
// {
// 	//close the socket but let the fd still on listening modus
// 	//https://stackoverflow.com/questions/27798419/closing-client-socket-and-keeping-server-socket-active
// 	//https://linux.die.net/man/2/sendto
// 	Log::printStringCol(CRITICAL, message);
// };