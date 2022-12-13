#include "../includes/Command.hpp"
#include <iostream>

bool Command::getCommandState(void) const { return this->_command_state; }			
bool Command::getReplyState(void) const { return this->_reply_state; }
std::string Command::getPrefix() const { return this->prefix; }
std::string Command::getUserCommand() const { return this->user_command; }
std::vector<std::string> Command::getParameters() { return this->_parameters; }
std::string Command::getQuery() { return this->_query; }

/* Splits a string by a delimiter and saves content in vector. */
std::vector<std::string> Command::split(std::string str, std::string delimiter)
{
	size_t pos;
	std::string tmp;
	std::vector<std::string> splitted_message;

	while ((pos = str.find(delimiter))!= std::string::npos)
	{
		tmp = str.substr(0, pos);
		splitted_message.push_back(tmp);
		str.erase(0, pos + delimiter.length());
	}

	if (str.length() != 0)
		splitted_message.push_back(str);

	return splitted_message;
}

/* Valid nickname consists of a-z and 0-9 and size <= 63 characters. */
int Command::check_characters(std::string str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (!isalpha(str.c_str()[i]) && !isdigit(str.c_str()[i]))		//Leerzeichen?
			return -1;
	}

	if (str.length() >= 63)
		return -1;

	return 1;
}

/* Loop through existing users and check if nickname is already taken. */
bool Command::check_free_nickname(const std::string& nickname)
{
	std::vector<User*> user_temp = this->_server->getUsers();

	for (std::vector<User*>::iterator iter = user_temp.begin(); iter != user_temp.end(); iter++)
	{
		if ((*iter)->getNickname() == nickname)
			return false;
	}

	return true;
}

std::string Command::getWelcomeReply(User* user)
{
	std::stringstream ss;
	ss << ":" << user->getNickUserHost() << " :Welcome to the 42-Queenz.42.fr network";
	return ss.str();
}

/* Register the user's nickname */
void Command::register_nickname(void)
{
	size_t param_size = this->_parameters.size();
	std::stringstream ss;

	//no nickname specified
	if (param_size == 1)
	{
		err_command(ERR_NONICKNAMEGIVEN);
		return ;
	}

	this->sender_nickname = this->_parameters[1];

	//check for characters/digits and length
	if (check_characters(this->_parameters[1]) < 0)
	{
		err_command(ERR_ERRONEUSNICKNAME);
		return ;
	}

	//once we can connect 2 users I can check this! -->check if another user is already using this nickname
	if (check_free_nickname(this->_parameters[1]) == false)
	{
		err_command(ERR_NICKNAMEINUSE);
		return ;
	}

	std::stringstream sstr;
	//send to all users in a channel where user is member / as well to user himself?
	sstr << ":" << this->_user->getNickname() << " changed their nickname to " << this->sender_nickname << "\r\n";
	this->_user->setNickname(this->sender_nickname);
	this->_reply_message = sstr.str();
	this->_reply_state = true;								//send reply to all users in channel when user is in chat
	this->_command_state = false;

	//necessary vector or reply messages
	if (this->_user->getNickname() != "Random_User" && this->_user->getUsername() != "Random_User")
	{
		this->_reply_message = getWelcomeReply(this->_user);
		this->_user->setState(REGISTERED);
	}

	this->_parameters.clear();
}

/* In case of an error does not send command to destination, but replies back to user in a 
   reply with a specified error string. */
void Command::err_command(std::string err_msg)
{
	this->_command_state = false;
	this->_reply_message = err_msg;
	this->_reply_state = true;
}

/* Split the received string and save command(cmd) and sending user(sender_nickname)  */
void Command::prepare_cmd(std::string message)
{
	this->_parameters = split(message, " ");

	//missing error handling in case of empty comand (only newline etc) or too less parameters
	this->user_command = this->_parameters[0];

	for (size_t i = 0; i < this->user_command.length(); i++)
		user_command[i] = std::toupper(user_command[i]);
}

void Command::register_username(void)
{
	//  Command: USER
  	//	Parameters: <username> 0 * <realname>
	// COMMAND <username> 0 * <realname>

	//question Kathi: can two users have the same username? weechat greps username automatically
	if (check_characters(this->_parameters[1]) < 0)
	{
		err_command(ERR_ERRONEUSNICKNAME);
		return ;
	}

	this->_user->setUsername(this->_parameters[1]);

	this->_parameters.clear();

	std::stringstream sstr;

	sstr << ":" << this->_user->getNickname() << " changed their nickname to " << this->sender_nickname << "\r\n";

	// if (this->_parameters[3] != "*" || this->_parameters[4] != "*")
	// {
	// 	std::cout << "ERROR: INVALID SYNTAX: USAGE </USER> <nickname> <* *> <:Fullname>";
	// 	return ;
	// }



	//send to all users in a channel where user is member / as well to user himself?
	this->_reply_message = sstr.str();
	this->_reply_state = true;								//send reply to all users in channel when user is in chat

	//necessary vector or reply messages
	if (this->_user->getNickname() != "Random_User" && this->_user->getUsername() != "Random_User")
	{
		this->_reply_message = getWelcomeReply(this->_user);
		this->_user->setState(REGISTERED);
	}



	this->_user->setUsername(this->sender_nickname);
	std::stringstream ss;

	for (size_t i = 0; i < this->_parameters.size(); i++)
		ss << this->_parameters[i];
	
	this->_user->setFullname(ss.str());

		if (this->_user->getNickname() != "Random_User" && this->_user->getUsername() != "Random_User")
	{
		this->_reply_message = getWelcomeReply(this->_user);
		this->_user->setState(REGISTERED);
	}



	// this->_user.setUsername()
}


Command::Command(User* user, Server* server, std::string message)
	: _user(user), _server(server), _query(message), _command_state(false), _reply_state(false)
{
	//if command starts with "\" then its a command. if it starts with : then its a reply 
	prepare_cmd(message);	//saves command in var this->user_command
	if (this->user_command.find(":") == 0)
	{
		std::cout << ("No command but a reply! No action needed by user.") << std::endl;
		return ;
	}

	// if (this->user_command.find("/") != 0)
	// 	err_command(ERR_UNKNOWNCOMMAND_CMD);
	if (this->user_command == "/NICK")
		register_nickname();
	else if (this->user_command == "/USER")
		register_username();
	else
		err_command(ERR_UNKNOWNCOMMAND_CMD);
}

/* Compiles now :) */
int	Command::find_user_in_server(const std::string nickname_receiver)
{
	std::vector<User*> user_temp = this->_server->getUsers();

	for (std::vector<User*>::iterator iter = user_temp.begin(); iter != user_temp.end(); iter++)
	{
		if ((*iter)->getNickname() == nickname_receiver)
			return 1;
	}
	Log::printStringCol(CRITICAL, "INVALID USER REQUEST. USER DOES NOT EXIST");
	return 0;

// VERSION BEFORE UPDATE KATHI
// KATHI: I wrote a getter function for users and store them in temp vector
// 	for (std::map<int, User*>::iterator iter = this->_server->_users.begin(); iter != this->_server->_users.end(); iter++)
// 	{
// 		if (iter->second->getNickname() == nickname_receiver)
// 		{
// 			this->user_receiver = nickname_receiver;			//in Command classe variable: User* user_receiver;
// 			return (1);
// 		}
// 		else if (iter == this->_server->_users.end())			// --> this can never be true as function exits for loop when iter == server._users.end()
// 		{
// 			std::cout << "INVALID USER REQUEST. USER DOES NOT EXIST" << std::endl;
// 			return (0);
// 		}
// 	}
// 	return (1);
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


	//KATHI Kommentar: Max hatte uns den Tipp gegeben mit std::stringstream zu arbeiten
	// std::stringstream ss;
	// ss << ":" << user->getNickname() << "!"<< user->getUsername()" << "@" << HOSTNAME << " " << msg;
	// user->setNickUserHost2(ss.str()); 	//um stringstream in normalen std::string umzuwandeln

	std::string ouput_to_client;
	ouput_to_client.append(":");
	ouput_to_client.append(user->getNickname());
	ouput_to_client.append("!");
	ouput_to_client.append(user->getNickname());
	ouput_to_client.append(HOSTNAME);
	ouput_to_client.append(" ");
	ouput_to_client.append(msg);
	user->setNickUserHost2(ouput_to_client);



	//? how do I check that printed by dest KATHY
	//std::cout << user->getNickname() << " | " << text << std::endl;
	//	//-> implement anstatt oben: ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
	Log::printStringCol(CRITICAL, msg);
};

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





















// -----------------		KATHI
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
// for (size_t i = 0; i < prefix.length(); i++)				//prefix in upper case characters
// 	prefix[i] = std::toupper(prefix[i]);
// this->_parameters.erase(_parameters.begin());			//delete prefix from vector