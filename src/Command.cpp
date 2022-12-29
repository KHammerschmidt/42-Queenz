#include "../includes/Command.hpp"

Command::~Command(void) {}
/* ======================================================================================== */
/* ---------------------------------- GETTERS/SETTERS ------------------------------------  */
bool Command::getCommandState(void) const { return this->command_state; }
bool Command::getReplyState(void) const { return this->reply_state; }
int Command::getReceiverFd(void) const { return this->receiver_fd; }

/* ======================================================================================== */
/* -------------------------------- CONSTRUCTOR MAIN LOOP  -------------------------------  */
Command::Command(User* user, Server* server, std::string message)
	: _user(user), _server(server), query(message), command_state(false), reply_state(false), user_command(""), channels_replies()
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
	else if (this->user_command == "PRIVMSG" || this->user_command == "NOTICE")
		sendPrivMsgUser(_user, query);
	else if (this->user_command == "JOIN")
		sendJoin(_user, query);
	else if (this->user_command == "PART")
		sendPart(query);
	else if (this->user_command == "MODE")
		sendMode(query);
	else if (this->user_command == "QUIT")
		sendQuit(_user);
	else
		err_command("421", message, ERR_UNKNOWNCOMMAND);

	if (this->_user->isRegistered() == true && this->_user->getState() != ONLINE)
	{
		getWelcomeReply(this->_user);
		this->_user->setState(ONLINE);
	}

	this->_args.clear();
}

/* ======================================================================================== */
/* -------------------------------------- PARSING  ---------------------------------------  */
/* Parse incoming string and extract command & nickname sender. */
bool Command::parse_command(std::string message)
{
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
}

void Command::register_cap(void)
{
	return ;
}

/* ======================================================================================== */
/* --------------------------------- REGISTER NICKNAME -----------------------------------  */
/* Register the user's nickname */
void Command::register_nickname(void)
{
	if (this->_user->_first_nick == false)
	{
		this->sender_nickname = this->_args[1];
		this->_user->_first_nick = true;
	}
	else
	{
		int index_of_first_space = query.find_first_of(" ");
		this->sender_nickname = query.substr(index_of_first_space + 1, query.length() - index_of_first_space);	//anstatt msg query
	}

	if (Utils::check_characters(this->_args[1]) < 0)
	{
		err_command("432", this->_user->getNickname(), ERR_ERRONEUSNICKNAME);
		return ;
	}

	if (check_free_nickname(this->_args[1]) == false && this->_user->getState() == ONLINE)
	{
		err_command("433", this->_user->getNickname(), ERR_NICKNAMEINUSE);
		return ;
	}

	// if (this->_user->getState() == ONLINE)
	// {
	// 	std::cout << "USER IS ONLINE: WITHIN NICK CHANGE" << std::endl;
	// 	//send this to all users in channel
	// }

	// std::stringstream ss;

	// ss << user->getNickUserHost() << " " << command << " #" << channel_name << "\r\n";
	// std::string a = ss.str();
	// for(std::multimap<std::string, User*>::iterator it=_server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
	// {
	// 	if (((*it).first).compare(channel_name) == 0)
	// 	{
	// 		int fd = (*it).second->getFd();
	// 		//std::cout << "FD: " << fd << " MESSAGE: " << a << "----------\n";
	// 		if (send(fd, a.c_str(), a.length(), 0) < 0)
	// 			Log::printStringCol(CRITICAL, "ERROR: SENDING REPLY TO USER FAIELD.");
	// 	}
	// }


	std::stringstream ss;
	std::string old_nick = this->_user->getNickname();
	this->_user->setNickname(this->sender_nickname);

	if (old_nick.length() == 0)
		ss << ":" << "*" << " NICK " << this->_user->getNickname() << " changed his nickname to " << sender_nickname << ".\r\n";			//send this to other users?
	else
		ss << ":" << old_nick << " NICK " << this->_user->getNickname() << " changed his nickname to " << sender_nickname << ".\r\n";		//send this to other users?

	this->_reply_message = ss.str();
	this->reply_state = true;
	this->command_state = false;
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

void Command::getWelcomeReply(User* user)
{
	std::stringstream ss;
	ss << ":" << HOSTNAME << " 001 " << user->getNickname() << " :Welcome to the 42-Queenz.42.fr network " << user->getNickUserHost() << "\r\n";

	this->_reply_message = ss.str();
	this->reply_state = true;
	this->command_state = false;
}

/* ======================================================================================== */
/* --------------------------------- REGISTER USERNAME -----------------------------------  */
void Command::register_username(void)
{
	//question Kathi: can two users have the same username? weechat greps username automatically
	// if (Utils::check_characters(this->_args[1]) < 0)
	// {
	// 	err_command(ERR_ERRONEUSNICKNAME);
	// 	return ;
	// }

	this->_user->setUsername(this->_args[2]);
	this->_user->setFullname(this->_args[3]);
}

/* ======================================================================================== */
/* ------------------------------ PING / PONG FUNCTIONS  ---------------------------------  */
void Command::send_pong(void)
{
	std::stringstream ss;

	ss << "PONG " << HOSTNAME << "\r\n";
	this->_reply_message = ss.str();
	this->reply_state = true;
	this->command_state = false;
}

/* ======================================================================================== */
/* --------------------------------------- UTILS  ----------------------------------------  */
User	*Command::return_user_in_server(const std::string nickname_receiver)
{
	std::vector<User*> temp = this->_server->getUsers();

	for (std::vector<User*>::iterator iter = temp.begin(); iter != temp.end(); iter++)
	{
		if ((*iter)->getNickname() == nickname_receiver)
			return (*iter);
	}
	return NULL;
}

/*NOTICE behaviour: like PRIVMSG,but:
if query not opened,
	print message in server;
otherwise p
	rint it in query.
*/

/* ======================================================================================== */
/* --------------------------------- PRIVATE MESSAGES COMMAND--------------------------  */
void Command::sendPrivMsgUser(User* user, std::string msg)
{
    int index_of_first_space;

	index_of_first_space = msg.find_first_of(" ");
	if (index_of_first_space == -1)
		return ;
	std::string command = msg.substr(0, index_of_first_space);
	std::string command_arg = msg.substr(index_of_first_space + 1, msg.length() - index_of_first_space);
	if (command.compare("PRIVMSG") != 0 && command.compare("NOTICE") != 0)
		return ;//print error

	index_of_first_space = command_arg.find_first_of(" ");
	if (index_of_first_space == -1)
		return ;
	std::string nick_receiver = command_arg.substr(0, index_of_first_space);
   	std::string text = command_arg.substr(index_of_first_space + 1, command_arg.length() - index_of_first_space);

	std::cout << "-----------TEST---------0\n";
	if (nick_receiver.substr(0, 1).compare("#") == 0)
	{
		sendChannelMsg(text, nick_receiver.substr(1, nick_receiver.length() -1));
		return ;
	}
	std::cout << "-----------TEST---------2\n";

	if (return_user_in_server(nick_receiver) == NULL)
		return ;

	std::stringstream ss;
	this->command_state = true;
	ss << user->getNickUserHost() << " " << command << " " << nick_receiver << " " << text << "\r\n";
	this->_command_message = ss.str();
	this->receiver_fd = return_user_in_server(nick_receiver)->getFd();

	Log::printStringCol(CRITICAL, msg);
}

/* ======================================================================================== */
/* --------------------------------- UTILITY--------- -----------------------------------  */
//create a string with all nicknames of users of this channel
std::string Command::return_string_all_users_in_channel(const std::string channel_name)
{//353
	std::stringstream ss;

	for(std::multimap<std::string, User*>::iterator it = _server->_channel_users.begin(); it != _server->_channel_users.end(); it++){
		if (((*it).first).compare(channel_name) == 0)
			ss << ((*it).second->getNickname()) << " ";
	}

	std::string s = ss.str();
	if (s.empty())
		return (NULL);
	s.erase(s.length() -1);
	return (s);
}

//check if a channel with this channel_name already exist
bool Command::new_channel_to_create(const std::string channel_name)
{
	bool new_channel_to_create = true;

	// for(std::multimap<std::string, User*>::iterator it = _server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
	// 	if (((*it).first).compare(channel_name) == 0)
	// 		new_channel_to_create = false ;

	// if (new_channel_to_create == false)
	// {
		for(std::vector<Channel*>::iterator it = _server->_channels.begin(); it != _server->_channels.end(); it++)
		{
			if (((*it)->getName()).compare(channel_name) == 0)
			{
				new_channel_to_create = false;
				(*it)->addUser(_user);
			}
		}
	// }

	return new_channel_to_create;
}

bool Command::joinInputFormatCheck(std::string command, std::string channel_name, std::string prefix_channel)
{
	//bool error = false;

	if (command.compare("JOIN") != 0 || prefix_channel.compare("#") !=0)
		return true;

	//test channel_name errors
	if (channel_name.find(" ") != (unsigned long) -1 || channel_name.find(":") != (unsigned long) -1 || channel_name.find(",") != (unsigned long) -1)
		return true;


	return false;
}


bool Command::handleDoubleUserError(std::string channel_name)
{
	//bool error= = false;

	if (_server->_channel_users.size() == 0)
		return false;

	for(std::multimap<std::string, User*>::iterator it = _server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
		if (((*it).first).compare(channel_name) == 0 && (((*it).second->getNickname()).compare(_user->getNickname()) == 0))
			return true;

	return false;

}

//check again, now users dont see each other in channel
/* ======================================================================================== */
/* --------------------------------- JOIN COMMAND -----------------------------------  */
void Command::sendJoin(User* user, const std::string msg)
{
	/*create function format msg, bis the resize part*/
	int index_of_first_space;

	index_of_first_space = msg.find_first_of(" ");
	if (index_of_first_space == -1)
		return ;
	std::string command = msg.substr(0, index_of_first_space);
	std::string channel_name = msg.substr(index_of_first_space + 2, msg.length() - index_of_first_space);
	std::string prefix_channel = msg.substr(index_of_first_space + 1, 1);

	if (channel_name.length() > 50)
    	    channel_name.resize(50);

	if (joinInputFormatCheck(command, channel_name, prefix_channel) == true)
		return ;

	if (handleDoubleUserError(channel_name) == true)
		return ;

	_server->_channel_users.insert(std::pair<std::string, User*> (channel_name, user));
	if (new_channel_to_create(channel_name) == true)
	{
		Channel* new_channel = new Channel(channel_name);
		new_channel->addUser(user);
		_server->_channels.push_back(new_channel);
		//_user->setNickname(_user->getNickname().insert(0, "@")); where put @ for ops?
	}

	this->reply_state = true;
	this->command_state = true;
	std::stringstream ss;

	ss << user->getNickUserHost() << " " << command << " #" << channel_name << "\r\n";
	std::string a = ss.str();
	for(std::multimap<std::string, User*>::iterator it=_server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
	{
		if (((*it).first).compare(channel_name) == 0)
		{
			int fd = (*it).second->getFd();
			//std::cout << "FD: " << fd << " MESSAGE: " << a << "----------\n";
			if (send(fd, a.c_str(), a.length(), 0) < 0)
				Log::printStringCol(CRITICAL, "ERROR: SENDING REPLY TO USER FAIELD.");
		}
	}

	this->reply_state = false;

	ss << ":" << HOSTNAME << " 332 " << user->getNickname() << " #" << channel_name << " :A timey wimey channel" << "\r\n";
	this->_command_message = ss.str();
	ss << ":" << HOSTNAME << " 353 " << user->getNickname() << " = #" << channel_name << " :@" << return_string_all_users_in_channel(channel_name) << "\r\n";//
	this->_command_message = ss.str();
	ss << ":" << HOSTNAME << " 366 " << user->getNickname() << " #" << channel_name << " End of NAMES list" << "\r\n";
	this->_command_message = ss.str();

	this->receiver_fd = user->getFd();
	std::cout << "COMMAND MESSAGE: " << this->_command_message;
	Log::printStringCol(CRITICAL, msg);

	std::cout <<  "------------------------------------------------------------------\n";
	std::cout <<  "------------------------------------------------------------------\n";
	std::cout <<  "------------------------------------------------------------------\n";
}//implement @ for OPERATORS USERS


bool Command::valid_channel(std::string channel_name)
{
	bool valid_channel = false;
	for(std::multimap<std::string, User*>::iterator it = _server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
	{
		if (((*it).first) == (channel_name))
		{
			valid_channel = true;
			break ;
		}
	}
	return valid_channel;
}
/* ======================================================================================== */
/* --------------------------------- CHANNEL MESSAGE COMMAND --------------------------  */
void Command::sendChannelMsg(std::string text, std::string channel_name)
{
	if (valid_channel(channel_name) == false)
		return ; //print error channel not exist //bug, if channel already opened by connecting to server, first user cannot write msg to server since channel is not known by every new connection from server side


	std::stringstream ss;
	ss << _user->getNickUserHost() << " PRIVMSG #" << channel_name << " " << text << "\r\n";
	std::string a = ss.str();
	for(std::multimap<std::string, User*>::iterator it=_server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
	{
		if (((*it).first).compare(channel_name) == 0 && ((*it).second->getNickname().compare(_user->getNickname()) != 0))
		{

           	// this->_command_message = ss.str();
			//this->receiver_fd = (*it).second->getFd();
			int fd = (*it).second->getFd();
			std::cout << "FD: " << fd << " MESSAGE: " << a << "----------\n";
			if (send(fd, a.c_str(), a.length(), 0) < 0)
				Log::printStringCol(CRITICAL, "ERROR: SENDING MESSAGE TO CHANNEL FAIELD.");
		}
	}
	this->command_state = false;
	this->reply_state = false;
	Log::printStringCol(CRITICAL, text);
}


/* ======================================================================================== */
/* --------------------------------- PART / LEAVE CHANNEL --------------------------  */
void Command::sendPart(std::string msg)
{
	int index_of_first_space;

	index_of_first_space = msg.find_first_of(" ");
	if (index_of_first_space == -1)
		return ; //print error and exit
	std::string command = msg.substr(0, index_of_first_space);
	std::string prefix_channel = msg.substr(index_of_first_space + 1, 1);
	std::string temp = msg.substr(index_of_first_space + 2, msg.length() - index_of_first_space);
	index_of_first_space = temp.find_first_of(" ");
	std::string text = temp.substr(index_of_first_space + 1, temp.length() - index_of_first_space);
	std::string channel_name = temp.substr(0, temp.length() - text.length() - 1);

	if (prefix_channel.compare("#") !=0 || valid_channel(channel_name) == false)
	 	return ;	//print error and exit

	std::stringstream ss;
	ss << _user->getNickUserHost() << " PART #" << channel_name << " " << text << "\r\n";
	std::string a = ss.str();
	for(std::multimap<std::string, User*>::iterator it=_server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
	{
		if (((*it).first).compare(channel_name) == 0 )
		{
			int fd = (*it).second->getFd();
			if (send(fd, a.c_str(), a.length(), 0) < 0)
				Log::printStringCol(CRITICAL, "ERROR: LEAVING CHANNEL MESSAGE TO CHANNEL FAILED.");
		}
	}
	this->command_state = false;
	this->reply_state = false;
}


//implement Kick separat
bool Command::find_user_in_channel(std::string channel_name, std::string nickname)
{
	if (_server->_channel_users.size() == 0)
		return false;

	for(std::multimap<std::string, User*>::iterator it = _server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
		if (((*it).first).compare(channel_name) == 0 && (((*it).second->getNickname()).compare(nickname) == 0))
			return true;

	return false;

}

//valid channel and return _channel are same, just one returns bool and other channel
Channel *Command::return_channel(std::string channel_name)
{
		for(std::vector<Channel*>::iterator it = _server->_channels.begin(); it != _server->_channels.end(); it++)
			if ((*it)->getName().compare(channel_name) == 0)
				return (*it);
		return NULL;
}

//continue MODE
void Command::setMode(std::string mode, std::string channel_name, std::string nickname){

	// mode = " ";
	// channel_name = " ";
	// nickname = " ";

	if (mode == "+o" || mode == "-o")										//change userop OBJ, usernotop STRING
		return_channel(channel_name)->giveTakeOpPrivileges(return_user_in_server(nickname), _user->getNickname(), mode);
	else if (mode == "+b")
		return_channel(channel_name)->deleteUser(return_user_in_server(nickname));
	else if (mode == "-b")
		return_channel(channel_name)->addUser(return_user_in_server(nickname));




}

//command prefix channel param[2] nickname

 //MODE #channel [PARAM] [+/-o nickname (OP) /  +b/-b nickname (BAN)]
//command channelname	  mode nickname
void Command::sendMode(std::string msg){
	int index_of_first_space;
	std::string mode;
	std::string nickname;
	std::string channel_name = "12";


	std::cout << "--------------TEST-------------1\n";
		// test user is op
	bool user_op = false;
	for(std::vector<Channel*>::iterator it = _server->_channels.begin(); it != _server->_channels.end(); it++)
	{
		if ((*it)->getName().compare(channel_name) == 0)
		{
			for(std::vector<User*>::iterator it2 = (*it)->_channel_operators.begin(); it2 != (*it)->_channel_operators.end(); it2++)
			{
				if ((*it2)->getNickname().compare(_user->getNickname()) == 0)
				{
					user_op = true;
					break;
				}
			}
		}
	}
	if (user_op == false)
	{std::cout << "User is not an Operator\n";	return;}
	std::cout << "--------------TEST-------------2\n";

	index_of_first_space = msg.find_first_of(" ");
	if (index_of_first_space == -1)
		return ; //print error and exit
	std::string command = msg.substr(0, index_of_first_space);
	std::string prefix_channel = msg.substr(index_of_first_space + 1, 1);
	channel_name = msg.substr(index_of_first_space + 2, msg.length() - index_of_first_space);//chnage and test

	if (prefix_channel.compare("#") !=0 || valid_channel(channel_name) == false)
		return ;	//print error and exit
	std::cout << "--------------TEST-------------2\n";

	index_of_first_space = channel_name.find_first_of(" ");
	mode = channel_name.substr(index_of_first_space + 1, channel_name.length() - index_of_first_space);
	index_of_first_space = text.find_first_of(" ");
	nickname = mode.substr(index_of_first_space + 1, mode.length() - index_of_first_space);
	if (find_user_in_channel(channel_name, nickname) == false)
		return; //error
	std::cout << "--------------TEST-------------3\n";



	// if (user_op == false)
	// 	ss << ((*it).second->getNickname()) << " ";
	// user_op = false;
		std::cout << "--------------TEST-------------4\n";





	//check if user is op

	if (mode.substr(0,2).compare("+o") == 0 || text.substr(0,2).compare("-o") == 0)
		setMode(mode, channel_name, nickname); //OP
	else if (mode.substr(0,2).compare("+b") == 0 || mode.substr(0,2).compare("-b") == 0 )
		setMode(mode, channel_name, nickname); //ban
	else
		return ; //error

}


//	/kick  nickname [reason] (KICK)

/* ======================================================================================== */
/* --------------------------------- QUIT COMMAND -----------------------------------  */
void Command::sendQuit(User* user)
{
	std::cout << " HERE QUIT" << std::endl;
	user->setState(DELETE);

	// close(user->getFd());
	// user->getNickname();
	//close the socket but let the fd still on listening modus
	//https://stackoverflow.com/questions/27798419/closing-client-socket-and-keeping-server-socket-active
	//Log::printStringCol(CRITICAL, message);
}

/* ======================================================================================== */
/* -------------------------------- HELPER FUNCTIONS  ------------------------------------  */
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
