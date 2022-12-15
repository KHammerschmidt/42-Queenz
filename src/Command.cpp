#include "../includes/Command.hpp"

/* ======================================================================================== */
/* ---------------------------------- GETTERS/SETTERS ------------------------------------  */
bool Command::getCommandState(void) const { return this->_command_state; }
bool Command::getReplyState(void) const { return this->_reply_state; }
std::string Command::getPrefix() const { return this->prefix; }
std::string Command::getUserCommand() const { return this->user_command; }
std::vector<std::string> Command::getParameters() { return this->_parameters; }
std::string Command::getQuery() { return this->_query; }


/* ======================================================================================== */
/* -------------------------------- CONSTRUCTOR MAIN LOOP  -------------------------------  */
Command::Command(User* user, Server* server, std::string message)
	: _user(user), _server(server), _query(message), authenticated(false), _command_state(false),	_reply_state(false)
{
	prepare_cmd(message);

	if (this->user_command == "PASS")
		register_pass();
	else if (this->user_command == "CAP")
		register_cap();
	else if (this->user_command == "NICK")
		register_nickname();
	else if (this->user_command == "USER")
		register_username();
	else if (this->user_command == "PING")
		send_pong();
	else
		err_command(ERR_UNKNOWNCOMMAND_CMD);

	// if (DBUG MSG)
	// {
		// if (this->user_command.find(":") == 0)
		// {
		// 	  std::cout << ("No command but a reply! No action needed by user.") << std::endl;
		// 	  return ;
		// }

		// if (this->user_command.find("/") != 0)	// how to check if first character is a /
		// 	err_command(ERR_UNKNOWNCOMMAND_CMD);
	// }

}

/* ======================================================================================== */
/* --------------------------------- REGISTER PASSWORD -----------------------------------  */
void Command::register_pass(void)
{
	if (this->_user->getPassword().length() != 0)
		std::cout << "ERROR: PASSWORD ALREADY SET." << std::endl;

	if (this->_server->getPassword().compare(this->_parameters[1]))
		this->_user->setPassword(this->_parameters[1]);
	else
		std::cout << "ERROR: INVALID PASSWORD." << std::endl;

	this->_parameters.clear();
	// this->authenticated = true;
}

void Command::register_cap(void)
{
	this->_parameters.clear();
	return ;
}


/* ======================================================================================== */
/* --------------------------------- REGISTER NICKNAME -----------------------------------  */
/* Register the user's nickname */
void Command::register_nickname(void)
{
	size_t param_size = this->_parameters.size();
	std::stringstream ss;

	if (param_size == 1)
	{
		err_command(ERR_NONICKNAMEGIVEN);
		return ;
	}

	this->sender_nickname = this->_parameters[1];

	// if (Utils::check_characters(this->_parameters[1]) < 0)
	// {
	// 	err_command(ERR_ERRONEUSNICKNAME);
	// 	return ;
	// }

	//once we can connect 2 users I can check this! -->check if another user is already using this nickname
	// if (check_free_nickname(this->_parameters[1]) == false)
	// {
	// 	err_command(ERR_NICKNAMEINUSE);
	// 	return ;
	// }

	this->_user->setNickname(this->sender_nickname);

	this->_reply_message = getWelcomeReply(this->_user);
	this->_reply_state = true;								//send reply to all users in channel when user is in chat
	this->_command_state = false;

	// if (this->_user->getNickname() != "Random_User" && this->_user->getUsername() != "Random_User")
	// {
		// this->_user->setState(REGISTERED);
	// }

	// this->_user->setState(REGISTERED);					//send reply to all users in channel when user is in chat
	this->_parameters.clear();
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
	ss << ":" << HOSTNAME << " 001 " << user->getNickname() << " :Welcome to the 42-Queenz.42.fr network " << user->getNickUserHost() << "\r\n";
	return ss.str();
}

/* ======================================================================================== */
/* --------------------------------- REGISTER USERNAME -----------------------------------  */
void Command::register_username(void)
{
	//  Command: USER
  	//	Parameters: <username> 0 * <realname>
	// COMMAND <username> 0 * <realname>

	//question Kathi: can two users have the same username? weechat greps username automatically
	// if (Utils::check_characters(this->_parameters[1]) < 0)
	// {
	// 	err_command(ERR_ERRONEUSNICKNAME);
	// 	return ;
	// }

	// this->_user->setUsername(this->_parameters[1]);

 	 this->_parameters.clear();



	// if (this->_parameters[3] != "*" || this->_parameters[4] != "*")
	// {
	// 	std::cout << "ERROR: INVALID SYNTAX: USAGE </USER> <nickname> <* *> <:Fullname>";
	// 	return ;
	// }

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


/* ======================================================================================== */
/* ------------------------------ PING / PONG FUNCTIONS  ---------------------------------  */
void Command::send_pong(void)
{
	std::cout << " PING RECEIVED " << std::endl;
	this->receiver_fd = this->_server->getServerFd();
	this->_command_message = this->_parameters[1];
	this->_command_state = true;
	this->_reply_state = false;
}


/* ======================================================================================== */
/* -------------------------------- HELPER FUNCTIONS  ------------------------------------  */
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
	this->_parameters = Utils::split(message, " ");

	//missing error handling in case of empty comand (only newline etc) or too less parameters
	std::vector<std::string> command = Utils::split(this->_parameters[0], "/");
	this->user_command = command[0];
	command.clear();

	for (size_t i = 0; i < this->user_command.length(); i++)
		user_command[i] = std::toupper(user_command[i]);
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
	if (command.compare("msg") != 0)
	{	std::cout << "error";
		return ;
	}

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
	if (command.compare("notice") != 0)
	{
		std::cout << "error";
		return ;
	}

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
void Command::sendJoin(User* user, const std::string msg)
{
	if (!user || msg.length() == 0)
		return ;

	int index_of_first_space;

	index_of_first_space = msg.find_first_of(" ");
	std::string command = msg.substr(1, index_of_first_space - 1);
	std::string command_arg = msg.substr(index_of_first_space + 1, msg.length() - index_of_first_space);
	if (command.compare("join") != 0)
	{
		std::cout << "error";
		return ;
	}


	//if command_arg[0] !="#" return (original message: ""Channel_name:": No such channel"); else check if channel already exist or not.
	if (command_arg[0] != '#')
		return ;

	//check if channel exist
		//channel_name.addUser(user);
	//else
		Channel::createChannel(msg);

	Log::printStringCol(CRITICAL, msg);
};


//void sendChannelMsg(User* user, std::string msg)
// {
		//formatierung: /msg #channel_name message
// }

// void Command::sendQuit(User* user, const std::string message)
// {
// 	//close the socket but let the fd still on listening modus
// 	//https://stackoverflow.com/questions/27798419/closing-client-socket-and-keeping-server-socket-active
// 	//https://linux.die.net/man/2/sendto
// 	Log::printStringCol(CRITICAL, message);
// };









// /* ======================================================================================== */
// /* -------------------------------- HELPER FUNCTIONS  ------------------------------------  */
// /* In case of an error does not send command to destination, but replies back to user in a
//    reply with a specified error string. */
// void Command::err_command(std::string err_msg)
// {
// 	this->_command_state = false;
// 	this->_reply_message = err_msg;
// 	this->_reply_state = true;
// }

// /* Split the received string and save command(cmd) and sending user(sender_nickname)  */
// void Command::prepare_cmd(std::string message)
// {
// 	this->_parameters = Utils::split(message, " ");

// 	//missing error handling in case of empty comand (only newline etc) or too less parameters
// 	this->user_command = this->_parameters[0];

// 	for (size_t i = 0; i < this->user_command.length(); i++)
// 		user_command[i] = std::toupper(user_command[i]);
// }
