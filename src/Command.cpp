#include "../includes/Command.hpp"

/* ======================================================================================== */
/* ---------------------------------- GETTERS/SETTERS ------------------------------------  */
bool Command::getCommandState(void) const { return this->command_state; }
bool Command::getReplyState(void) const { return this->reply_state; }
std::string Command::getPrefix() const { return this->prefix; }
std::string Command::getUserCommand() const { return this->user_command; }
std::vector<std::string> Command::getParameters() { return this->_args; }
std::string Command::getQuery() { return this->query; }


/* ======================================================================================== */
/* -------------------------------- CONSTRUCTOR MAIN LOOP  -------------------------------  */
Command::Command(User* user, Server* server, std::string message)
	: _user(user), _server(server), query(message), authenticated(false), command_state(false),	reply_state(false), user_command("")
{
	if (!parse_command(message))
		return ;

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
	else if (this->user_command == "PRIVMSG")
		sendPrivMsgUser(_user, query);
	else
		err_command("421", message, ERR_UNKNOWNCOMMAND);

	// if (DBUG MSG) 	// {
		// if (this->user_command.find(":") == 0)
		// {
		// 	  std::cout << ("No command but a reply! No action needed by user.") << std::endl;
		// 	  return ;
		// }
		// if (this->user_command.find("/") != 0)	// how to check if first character is a /
		// 	err_command(ERR_UNKNOWNCOMMAND_CMD);
	// }

	this->_args.clear();
}


/* ======================================================================================== */
/* --------------------------------- REGISTER PASSWORD -----------------------------------  */
void Command::register_pass(void)
{
	if (this->_user->getPassword().length() != 0)
	{
		err_command("462", user_command, ERR_ALREADYREGISTERED);
		return ;
	}

	if (this->_args[0].length() == 0)
	{
		err_command("461", user_command, ERR_NEEDMOREPARAMS);
		return ;
	}

	if (this->_server->getPassword().compare(this->_args[1]))
	{
		err_command("464", user_command, ERR_PASSWDMISMATCH);
		return ;
	}

	this->_user->setPassword(this->_args[1]);
	if (this->_user->getNickname().length() != 0 && this->_user->getUsername().length() != 0 && this->_user->getPassword().length() != 0)
	{
		getWelcomeReply(this->_user);
		this->_user->setState(REGISTERED);
	}
}

void Command::register_cap(void)
{
	this->_args.clear();
	return ;
}


/* ======================================================================================== */
/* --------------------------------- REGISTER NICKNAME -----------------------------------  */
/* Register the user's nickname */
void Command::register_nickname(void)
{
	// size_t param_size = this->_args.size();
	std::stringstream ss;

	// if (param_size == 1)
	// {
	// 	err_command(ERR_NONICKNAMEGIVEN);
	// 	return ;
	// }

	this->sender_nickname = this->_args[1];

	// if (!param_size)
	// 	return ;
	// if (Utils::check_characters(this->_args[1]) < 0)
	// {
	// 	err_command(ERR_ERRONEUSNICKNAME);
	// 	return ;
	// }

	//once we can connect 2 users I can check this! -->check if another user is already using this nickname
	// if (check_free_nickname(this->_args[1]) == false)
	// {
	// 	err_command(ERR_NICKNAMEINUSE);
	// 	return ;
	// }

	this->_user->setNickname(this->sender_nickname);

	this->_reply_message = getWelcomeReply(this->_user);
	this->reply_state = true;								//send reply to all users in channel when user is in chat
	this->command_state = false;
	if (this->_user->getNickname().length() != 0 && this->_user->getUsername().length() != 0 && this->_user->getPassword().length() != 0)
	{
		getWelcomeReply(this->_user);
		this->_user->setState(REGISTERED);
	}

	// if (this->_user->getNickname().length() != 0 && this->_user->getUsername().length() != 0 && this->_user->getPassword().length() != 0)
	// {
	// 	getWelcomeReply(this->_user);
	// 	this->_user->setState(REGISTERED);
	// }

	// this->_user->setState(REGISTERED);					//send reply to all users in channel when user is in chat
	this->_args.clear();
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
	// if (Utils::check_characters(this->_args[1]) < 0)
	// {
	// 	err_command(ERR_ERRONEUSNICKNAME);
	// 	return ;
	// }

	// this->_user->setUsername(this->_args[1]);

 	//  this->_args.clear();



	// if (this->_args[3] != "*" || this->_args[4] != "*")
	// {
	// 	std::cout << "ERROR: INVALID SYNTAX: USAGE </USER> <nickname> <* *> <:Fullname>";
	// 	return ;
	// }

	// std::stringstream ss;
	// for (size_t i = 0; i < this->_args.size(); i++)
	// 	ss << this->_args[i];

	// this->_user->setFullname(ss.str());

	this->_user->setUsername(this->_args[2]);
	this->_user->setFullname(this->_args[3]);
	if (this->_user->getNickname().length() != 0 && this->_user->getUsername().length() != 0 && this->_user->getPassword().length() != 0)
	{
		getWelcomeReply(this->_user);
		this->_user->setState(REGISTERED);
	}

	// if (this->_user->getNickname().length() != 0 && this->_user->getUsername().length() != 0 && this->_user->getPassword().length() != 0)
	// {
	// 	getWelcomeReply(this->_user);
	// 	this->_user->setState(REGISTERED);
	// }
	// this->_reply_message = getWelcomeReply(this->_user);

	// this->_user.setUsername()
}


/* ======================================================================================== */
/* ------------------------------ PING / PONG FUNCTIONS  ---------------------------------  */
void Command::send_pong(void)
{
	std::cout << " PING RECEIVED " << std::endl;
	this->receiver_fd = this->_server->getServerFd();
	this->_command_message = this->_args[1];
	this->command_state = true;
	this->reply_state = false;
}


/* ======================================================================================== */
/* -------------------------------- HELPER FUNCTIONS  ------------------------------------  */
/* Parse incoming string and extract command & nickname sender. */
bool Command::parse_command(std::string message)
{
	//how to check how many params a command needs?		--> gibt es einen command bei dem nur ein PARAM ausreicht???
	if (message.find(" ") == std::string::npos)
	{
		this->_reply_message = put_reply_cmd(this->_user, "461", message, ERR_NEEDMOREPARAMS);
		this->reply_state = true;
		this->command_state = false;
		return false;
	}

	this->_args = Utils::split(message, " ");
	message = "";

	if (this->_args[0].find("/") != std::string::npos)
		this->user_command = this->_args[0].substr(1, this->_args[0].length() - 1);
	else
		this->user_command = this->_args[0].substr(0, this->_args[0].length());

	for (size_t i = 0; i < this->user_command.length(); i++)
		user_command[i] = std::toupper(user_command[i]);

	this->_args.erase(this->_args.begin());
	this->sender_nickname = this->_args[0];
	this->_args.erase(this->_args.begin());

	return true;
}


std::string Command::put_reply_cmd(User* user, std::string err_num, std::string cmd, std::string code)
{
	std::stringstream ss;
	ss << err_num << " " << user->getNickname() << cmd << " :" << code;
	return ss.str();
}

std::string Command::put_reply(User* user, std::string err_num, std::string code)
{
	std::stringstream ss;
	ss << err_num << " " << user->getNickname() << " :" << code;
	return ss.str();
}


/* In case of an error does not send command to destination, but replies back to user with error string. */
void Command::err_command(std::string err_num, std::string cmd, std::string code)
{
	if (cmd.length() == 0)
		this->_reply_message = put_reply(this->_user, err_num, code);
	else
		this->_reply_message = put_reply_cmd(this->_user, err_num, cmd, code);

	this->command_state = false;
	this->reply_state = true;
}

void Command::print_vector(std::vector<std::string> vctr)
{
	std::vector<std::string>::iterator iter;
	for (iter = vctr.begin(); iter != vctr.end(); iter++)
		std::cout << *iter << std::endl;
}




User	*Command::return_user_in_server(const std::string nickname_receiver)
{
	std::vector<User*> user_temp = this->_server->getUsers();

	for (std::vector<User*>::iterator iter = user_temp.begin(); iter != user_temp.end(); iter++)
	{
		if ((*iter)->getNickname() == nickname_receiver)
			return (*iter);
	}
	return NULL;
	//throw error
}


int	Command::find_user_in_server(const std::string nickname_receiver)
{
	std::vector<User*> user_temp = this->_server->getUsers();

	// //
	// std::cout << "-----------";
	// for (std::vector<User*>::iterator iter = user_temp.begin(); iter != user_temp.end(); iter++)
	// {
	// 	std::cout << ((*iter)->getNickname()) << "-----------" << std::endl;
	// }
	// //

	for (std::vector<User*>::iterator iter = user_temp.begin(); iter != user_temp.end(); iter++)
	{
		if ((*iter)->getNickname() == nickname_receiver)
			return 1;
	}

	Log::printStringCol(CRITICAL, "INVALID USER REQUEST. USER DOES NOT EXIST");
	return 0;
}


void Command::sendPrivMsgUser(User* user, std::string msg)		
{				
	//std::string msg = "/PRIVMSG ben ciao";
    int index_of_first_space;

	index_of_first_space = msg.find_first_of(" ");
	std::string command = msg.substr(0, index_of_first_space);
	std::string command_arg = msg.substr(index_of_first_space + 1, msg.length() - index_of_first_space);
	if (command.compare("PRIVMSG") != 0)
	{	std::cout << command;		
		return ;
	}

	//find  first space to have lenght of nick
	index_of_first_space = command_arg.find_first_of(" ");
	if (!index_of_first_space)
		return ;
	std::string nick_receiver = command_arg.substr(0, index_of_first_space);
	//test
	// if (nick_receiver.compare("b") != 0)
	// {	std::cout << nick_receiver << "++++++++";
	// 	return ;
	// }
	//std::cout << nick_receiver << "-------";

	if (find_user_in_server(nick_receiver) == 0)
		return ;


	//text to print
   	std::string text = command_arg.substr(index_of_first_space + 1, command_arg.length() - index_of_first_space);


	//KATHI Kommentar: Max hatte uns den Tipp gegeben mit std::stringstream zu arbeiten
	// std::stringstream ss;
	// ss << ":" << user->getNickname() << "!"<< user->getUsername()" << "@" << HOSTNAME << " " << msg;
	// user->setNickUserHost2(ss.str()); 	//um stringstream in normalen std::string umzuwandel


	std::stringstream ss;
	this->command_state = true;
	ss << user->getNickUserHost() << " PRIVMSG " << nick_receiver << " :" << text << "\r\n";
	this->_command_message = ss.str();
	this->receiver_fd = return_user_in_server(nick_receiver)->getFd();
		
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

	this->command_state = true;
	this->reply_state = true;

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
