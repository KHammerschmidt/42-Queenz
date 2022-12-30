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

	if (this->_user->isRegistered() == false)
	{
		if (this->user_command == "PASS")
			register_pass();
		else if (this->user_command == "CAP" )
			register_cap();
		else if (this->user_command == "NICK")
			register_nickname();
		else if (this->user_command == "USER")
			register_username();
		else
		{
			err_command("464", user_command, "You must register first with a password.\r\n");
			// this->_user->setState(DELETE);
			// err_command("421", message, ERR_UNKNOWNCOMMAND);
		}
	}
	else
	{
		if (query.find("\r") != std::string::npos)
			query = query.substr(0, query.length() - 1);
		if (this->user_command == "NICK")
			register_nickname();
		else if (this->user_command == "PING")
			send_pong();
		else if (this->user_command == "PRIVMSG" || this->user_command == "NOTICE")
			sendPrivMsgUser(_user, query);
		else if (this->user_command == "JOIN")
			sendJoin(_user, query);
		else if (this->user_command == "TOPIC")
			sendTopic(query);
		else if (this->user_command == "PART")
			sendPart(query);
		else if (this->user_command == "MODE")
			sendMode(query);
		else if (this->user_command == "INVITE")
			sendInvite(query);
		else if (this->user_command == "KICK")
			sendKick(query);
		else if (this->user_command == "QUIT")
			sendQuit(_user);
		else
			err_command("421", message, ERR_UNKNOWNCOMMAND);
	}

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
	if (message.find(" ") == std::string::npos && msg_quit(message) == false)
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

	return true;
}

/* Checks if single command is quit. */
bool Command::msg_quit(std::string message)
{
	if (message == "QUIT" || message == "quit")
		return true;

	return false;
}

/* ======================================================================================== */
/* --------------------------------- REGISTER PASSWORD -----------------------------------  */
void Command::register_pass(void)
{
	std::string new_pw;

	if (this->_args[0].find("\r") != std::string::npos)
		new_pw = this->_args[0].substr(0, this->_args[0].length() - 1);
	else
		new_pw = this->_args[0].substr(0, this->_args[0].length());

	if (this->_user->getPassword() == new_pw)
	{
		err_command("462", user_command, ERR_ALREADYREGISTERED);
		return ;
	}

	if (this->_server->getPassword() != new_pw)
	{
		err_command("464", user_command, ERR_PASSWDMISMATCH);
		return ;
	}

	this->_user->setPassword(new_pw);
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
		this->sender_nickname = this->_args[0];
		this->_user->_first_nick = true;
	}
	else
	{
		int index_of_first_space = query.find_first_of(" ");
		this->sender_nickname = query.substr(index_of_first_space + 1, query.length() - index_of_first_space);
	}

	if (this->_user->getPassword() != this->_server->getPassword())
	{
		err_command("464", user_command, "You must register first with a password.\r\n");
		return ;
	}

	std::string new_nick;
	if (this->_args[0].find("\r") != std::string::npos)
		new_nick = this->_args[0].substr(0, this->_args[0].length() - 1);
	else
		new_nick = this->_args[0].substr(0, this->_args[0].length());

	if (this->_user->getNickname() == new_nick)
		return ;

	if (Utils::check_characters(new_nick) < 0)
	{
		err_command("432", this->_user->getNickname(), ERR_ERRONEUSNICKNAME);
		return ;
	}

	if (check_free_nickname(new_nick) == false)
	{
		err_command("433", this->_user->getNickname(), ERR_NICKNAMEINUSE);
		return ;
	}

	std::stringstream ss;
	std::string old_nick = "*";

	if (this->_user->getNickname().length() != 0)
		old_nick = this->_user->getNickname();

	ss << ":" << old_nick << " " << user_command << " " << new_nick << "\r\n";

	this->_user->setNickname(new_nick);

	this->_reply_message = ss.str();
	this->reply_state = true;
	this->command_state = false;

	send_replies_to_channel();
}

/* Sends a specified string to all members of all channels to which the current user belongs. */
void Command::send_replies_to_channel(void)
{
	std::vector<Channel*>::iterator it;
	for (it = _server->_channels.begin(); it != _server->_channels.end(); it++)
	{
		Channel* tmp_channel = *it;
		std::cout << tmp_channel->getName() << std::endl;

		for(std::multimap<std::string, User*>::iterator iter =_server->_channel_users.begin(); iter != _server->_channel_users.end(); iter++)
		{
			if (((*iter).first).compare(tmp_channel->getName()) == 0)
			{
				int fd = (*iter).second->getFd();
				if (send(fd, _reply_message.c_str(), _reply_message.length(), 0) < 0)
					Log::printStringCol(CRITICAL, "ERROR: SENDING REPLY TO USER FAILED hihihi.");
				else
					Log::printStringCol(LOG, "LOG: SENDING REPLY SUCCESSFUL hihii.");
			}
		}
	}
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
	if (this->_user->getPassword() != this->_server->getPassword())
	{
		err_command("464", user_command, "You must register first with a password.\r\n");
		return ;
	}

	if (Utils::check_characters(this->_args[1]) < 0)
	{
		err_command("432", this->_user->getNickname(), ERR_ERRONEUSUSERNAME);
		return ;
	}

	std::string new_username;
	if (this->_args[0].find("\r") != std::string::npos)
		new_username = this->_args[0].substr(0, this->_args[0].length() - 1);
	else
		new_username = this->_args[0].substr(0, this->_args[0].length());

	this->_user->setUsername(new_username);
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

	if (nick_receiver.substr(0, 1).compare("#") == 0)
	{
		sendChannelMsg(text, nick_receiver.substr(1, nick_receiver.length() -1));
		return ;
	}

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
	// if (channel_name.find("\r") != std::string::npos)
	// 	channel_name = channel_name.substr(0, channel_name.length() - 1);


	std::string prefix_channel = msg.substr(index_of_first_space + 1, 1);

	if (channel_name.length() > 50)
    	    channel_name.resize(50);

	std::cout << "CHANNEL_NAME: " << channel_name.find("\r") << "\n";
	if (joinInputFormatCheck(command, channel_name, prefix_channel) == true)
		return ;

	if (handleDoubleUserError(channel_name) == true)
		return ;


	//_server->_channel_users.insert(std::pair<std::string, User*> (channel_name, user));
	_server->_channel_users.insert(std::make_pair(channel_name, user));
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
		if (((*it).first).compare(channel_name) == 0 && (*it).second->getNickname().compare(_user->getNickname()) != 0)
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
	//delete channel/user from multimap
	for(std::multimap<std::string, User*>::iterator it=_server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
	{
		if ((*it).first.compare(channel_name) == 0  && (*it).second->getNickname().compare(_user->getNickname()) == 0 )
		{
			_server->_channel_users.erase(it);
			break ;
		}
	}
	//delete channel members/operators from channel
	for(std::vector<Channel*>::iterator it = _server->_channels.begin(); it != _server->_channels.end(); it++)
	{
		if ((*it)->getName().compare(channel_name) == 0 )
			(*it)->deleteUser(_user);
	}

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


User *Command::return_user_in_multimap(std::string channel_name, std::string nickname)
{
	for(std::multimap<std::string, User*>::iterator it=_server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
	{
		if (((*it).first).compare(channel_name) == 0 && (*it).second->getNickname().compare(nickname) == 0)
			return (it)->second;
	}
	return NULL;
}

std::string Command::return_string_all_users_OP_in_channel(const std::string channel_name)
{//353
	std::stringstream ss;

	for(std::multimap<std::string, User*>::iterator it = _server->_channel_users.begin(); it != _server->_channel_users.end(); it++){
		if (((*it).first).compare(channel_name) == 0)
		{
			if ((*it).second->getNicknameOP().substr(0,1).compare("@") == 0)
				ss << ((*it).second->getNicknameOP()) << " ";
			else
				ss << ((*it).second->getNickname()) << " ";
			std::cout << "NICK OP: "<< (*it).second->getNicknameOP() << "\n";
		}
	}

	std::string s = ss.str();
	if (s.empty())
		return (NULL);
	s.erase(s.length() -1);
	return (s);
}


//continue MODE
//? how to update @ in channel? maybe change Nick
void Command::setMode(std::string mode, std::string channel_name, std::string nickname){

	if (mode == "+o" || mode == "-o")
		return_channel(channel_name)->giveTakeOpPrivileges(return_user_in_multimap(channel_name, nickname), return_user_in_multimap(channel_name, _user->getNickname()), mode);						//change userop OBJ, usernotop STRING

	//privilege schon getestet in function davor->	//return_channel(channel_name)->giveTakeOpPrivileges(return_user_in_server(nickname), _user->getNickname(), mode);

	std::stringstream ss;
	ss << ":" << HOSTNAME << " 353 " << _user->getNickname() << " = #" << channel_name << " :" << return_string_all_users_OP_in_channel(channel_name) << "\r\n";//
	ss << ":" << HOSTNAME << " 366 " << _user->getNickname() << " #" << channel_name << " End of NAMES list" << "\r\n";

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
}

//command prefix channel param[2] nickname

 //MODE #channel [PARAM] [+/-o nickname (OP) /  +b/-b nickname (BAN)]
//command channelname	  mode nickname
void Command::sendMode(std::string msg){
	int index_of_first_space;
	std::string mode;
	std::string nickname;


	// test user is op

	 if (msg.find("\r") != std::string::npos)
	 	std::cout << "@@@@@@@@@@@@@@@@@@@\n";

	index_of_first_space = msg.find_first_of(" ");
	if (index_of_first_space == -1)
		return ; //print error and exit
	std::string command = msg.substr(0, index_of_first_space);
	std::string prefix_channel = msg.substr(index_of_first_space + 1, 1);
	std::string temp = msg.substr(index_of_first_space + 2, msg.length() - index_of_first_space);//chnage and test
	index_of_first_space = temp.find_first_of(" ");
	if (index_of_first_space == -1)
		return ; //print error and exit
	std::string channel_name = temp.substr(0, temp.length() - (temp.length() - index_of_first_space));
	temp = temp.substr(index_of_first_space + 1, temp.length() - (channel_name).length() - 1); /*mode + nick*/
	index_of_first_space = temp.find_first_of(" ");
	if (index_of_first_space == -1)
		return ; //print error and exit
	mode = temp.substr(0, index_of_first_space);
	nickname = temp.substr(index_of_first_space + 1, temp.length() - mode.length() - 1);//chnage and test
	std::cout << "NICKNAME: " << nickname << "LENGHT: " << nickname.length() << "\n";
	//std::cout << "command: " << command << "\n-----prefix: " << prefix_channel << "\n--------channel_name: " << channel_name << "\n--------mode: " << mode << "\n------nickname: " << nickname << "\n--------------TEST-------------n\n";


	if (prefix_channel.compare("#") !=0 || valid_channel(channel_name) == false)
		return ;	//print error and exit

	//check nickname is in channel
	if (find_user_in_channel(channel_name, nickname) == false)
		return; //error


	//check if user this is an operator -> can just check getNickOP==true
	bool user_op = false;
	for(std::vector<Channel*>::iterator it = _server->_channels.begin(); it != _server->_channels.end(); it++)
	{

		if ((*it)->getName().compare(channel_name) == 0)
		{
			// for(std::vector<User*>::iterator it2 = (*it)->_channel_operators.begin(); it2 != (*it)->_channel_operators.end(); it2++)
			// 	std::cout << (*it2)->getNickname();
			for(std::vector<User*>::iterator it2 = (*it)->_channel_operators.begin(); it2 != (*it)->_channel_operators.end(); it2++)
			{
				if ((*it2)->getNickname().compare(_user->getNickname()) == 0)
				{
					user_op = true;
					break;
				}
			}
			if (user_op == false)
			{
				std::cout << "User is not an Operator\n";
				return;
			}
			break;
		}
	}




	//check if mode is legit
	if (mode.substr(0,2).compare("+o") == 0 || mode.substr(0,2).compare("-o") == 0)
		setMode(mode, channel_name, nickname); //OP
	else if (mode.substr(0,2).compare("+b") == 0 || mode.substr(0,2).compare("-b") == 0 )
		setMode(mode, channel_name, nickname); //ban
	else
		return ; //error

}

/*  TOPIC #test :another topic*/
void Command::sendTopic(std::string msg)
{
	int index_of_first_space;
	std::string mode;
	std::string topic;
	std::stringstream ss;

	// test user is op


	index_of_first_space = msg.find_first_of(" ");
	if (index_of_first_space == -1)
		return ; //print error and exit

	std::string command = msg.substr(0, index_of_first_space);
	std::string prefix_channel = msg.substr(index_of_first_space + 1, 1);

  std::string temp = msg.substr(index_of_first_space + 2, msg.length() - index_of_first_space);//chnage and test
	index_of_first_space = temp.find_first_of(" ");
	if (index_of_first_space == -1)
		return ; //print error and exit

	std::string channel_name = temp.substr(0, temp.length() - (temp.length() - index_of_first_space));
	topic = temp.substr(index_of_first_space + 2, temp.length() - (channel_name).length() - 1); /*mode + nick*/
	index_of_first_space = temp.find_first_of(" ");
	std::cout << " topic:" << topic << "\ncommand: "  << command << "\n-----prefix: " << prefix_channel << "\n--------channel_name: " << channel_name << "\n--------topic: " << topic << "\n--------------TEST-------------n\n";


	//check if user this is an operator -> can just check getNickOP==true
	bool user_op = false;
	for(std::vector<Channel*>::iterator it = _server->_channels.begin(); it != _server->_channels.end(); it++)
	{

		if ((*it)->getName().compare(channel_name) == 0)
		{
			// for(std::vector<User*>::iterator it2 = (*it)->_channel_operators.begin(); it2 != (*it)->_channel_operators.end(); it2++)
			// 	std::cout << (*it2)->getNickname();
			for(std::vector<User*>::iterator it2 = (*it)->_channel_operators.begin(); it2 != (*it)->_channel_operators.end(); it2++)
			{
				if ((*it2)->getNickname().compare(_user->getNickname()) == 0)
				{
					user_op = true;
					break;
				}
			}
			if (user_op == false)
			{
				std::cout << "User is not an Operator\n";
				return;
			}
			break;
		}
	}

	ss << ":" << HOSTNAME << " 332 " << _user->getNickname() << " #" << channel_name << " " << topic << "\r\n";
	std::string a = ss.str();

	for(std::multimap<std::string, User*>::iterator it=_server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
	{
		if (((*it).first).compare(channel_name) == 0)
		{
				int fd = (*it).second->getFd();
				if (send(fd, a.c_str(), a.length(), 0) < 0)
					Log::printStringCol(CRITICAL, "ERROR: SENDING TOPIC TO CHANNEL FAIELD.");
		}
	}



}


//	/kick #channel  nickname [reason] (KICK)
void Command::sendKick(std::string msg)
{
	int index_of_first_space;
	std::string mode;
	std::string nickname;
	std::stringstream ss;


	// test user is op


	index_of_first_space = msg.find_first_of(" ");
	if (index_of_first_space == -1)
		return ; //print error and exit
	std::string command = msg.substr(0, index_of_first_space);
	std::string prefix_channel = msg.substr(index_of_first_space + 1, 1);
	std::string temp = msg.substr(index_of_first_space + 2, msg.length() - index_of_first_space);//chnage and test
	index_of_first_space = temp.find_first_of(" ");
	if (index_of_first_space == -1)
		return ; //print error and exit
	std::string channel_name = temp.substr(0, temp.length() - (temp.length() - index_of_first_space));
	temp = temp.substr(index_of_first_space + 1, temp.length() - (channel_name).length() - 1); /*mode + nick*/
	index_of_first_space = temp.find_first_of(" ");
	if (index_of_first_space == -1)
		return ; //print error and exit
	nickname = temp.substr(0, index_of_first_space);
	std::string text = temp.substr(index_of_first_space + 1, temp.length() - mode.length() - 1);//chnage and test
	std::cout << "command: " << command << "\n-----prefix: " << prefix_channel << "\n--------channel_name: " << channel_name << "\n--------mode: " << mode << "\n------nickname: " << nickname << "\n--------------TEST-------------n\n";


	if (prefix_channel.compare("#") !=0 || valid_channel(channel_name) == false)

		return ;	//print error and exit

	//check nickname is in channel
	if (find_user_in_channel(channel_name, nickname) == false)
		return; //error


	//check if user this is an operator -> can just check getNickOP==true
	bool user_op = false;
	for(std::vector<Channel*>::iterator it = _server->_channels.begin(); it != _server->_channels.end(); it++)
	{

		if ((*it)->getName().compare(channel_name) == 0)
		{
			// for(std::vector<User*>::iterator it2 = (*it)->_channel_operators.begin(); it2 != (*it)->_channel_operators.end(); it2++)
			// 	std::cout << (*it2)->getNickname();
			for(std::vector<User*>::iterator it2 = (*it)->_channel_operators.begin(); it2 != (*it)->_channel_operators.end(); it2++)
			{
				if ((*it2)->getNickname().compare(_user->getNickname()) == 0)
				{
					user_op = true;
					break;
				}
			}
			if (user_op == false)
			{
				std::cout << "User is not an Operator\n";
				return;
			}
			break;
		}
	}

	ss << _user->getNickUserHost() << " KICK #" << channel_name << " " << nickname << " " << text << "\r\n";
	std::string a = ss.str();

	for(std::multimap<std::string, User*>::iterator it = _server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
	{
		if (((*it).first).compare(channel_name) == 0)
		{
				int fd = (*it).second->getFd();
				if (send(fd, a.c_str(), a.length(), 0) < 0)
					Log::printStringCol(CRITICAL, "ERROR: KICKING USER FROM CHANNEL FAIELD.");

		}
	}
	/*CANCEL USER FROM MULTIMAP*/
	for(std::multimap<std::string, User*>::iterator it = _server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
	{
		if (((*it).first).compare(channel_name) == 0 && (*it).second->getNickname().compare(nickname) == 0)
		{
			_server->_channel_users.erase(it);
			break;
		}
	}

}

// :Angel!wings@irc.org INVITE Wiz #Dust
//      Command: INVITE <nickname> <channel>

void Command::sendInvite(std::string msg)
{
	int index_of_first_space;
	std::string nickname;
	std::stringstream ss;


	std::cout << "--------------TEST-------------1\n";
	// test user is op


	index_of_first_space = msg.find_first_of(" ");
	if (index_of_first_space == -1)
		return ; //print error and exit
	std::string command = msg.substr(0, index_of_first_space);
	std::string temp = msg.substr(index_of_first_space + 1, msg.length() - index_of_first_space);//chnage and test
	index_of_first_space = temp.find_first_of(" ");



	if (index_of_first_space == -1)
		return ; //print error and exit
	nickname = temp.substr(0, temp.length() - (temp.length() - index_of_first_space));
	index_of_first_space = temp.find_first_of(" ");
	if (index_of_first_space == -1)
		return ; //print error and exit
	temp = temp.substr(index_of_first_space + 1, temp.length() - (nickname).length() - 1); /*mode + nick*/
	std::string prefix_channel = temp.substr(0, 1);
	std::string channel_name = temp.substr(1, index_of_first_space);

	std::cout << "command: " << command << "\n-----nickname: " << nickname << "\n--------channel_name: " << channel_name <<  "\n--------------TEST-------------n\n";


	if (prefix_channel.compare("#") !=0 || valid_channel(channel_name) == false)
		return ;	//print error and exit

	//check nickname is in channel
	if (find_user_in_channel(channel_name, _user->getNickname()) == false)
		return; //error





	//check if user this is an operator -> can just check getNickOP==true
	bool user_op = false;
	for(std::vector<Channel*>::iterator it = _server->_channels.begin(); it != _server->_channels.end(); it++)
	{

		if ((*it)->getName().compare(channel_name) == 0)
		{
			// for(std::vector<User*>::iterator it2 = (*it)->_channel_operators.begin(); it2 != (*it)->_channel_operators.end(); it2++)
			// 	std::cout << (*it2)->getNickname();
			for(std::vector<User*>::iterator it2 = (*it)->_channel_operators.begin(); it2 != (*it)->_channel_operators.end(); it2++)
			{
				if ((*it2)->getNickname().compare(_user->getNickname()) == 0)
				{
					user_op = true;
					break;
				}
			}
			if (user_op == false)
			{
				std::cout << "User is not an Operator\n";
				return;
			}
			break;
		}
	}
// :Angel!wings@irc.org INVITE Wiz #Dust

	ss << _user->getNickUserHost() << " INVITE " << nickname << " #" << channel_name << "\r\n";
	std::string a = ss.str();

	for(std::multimap<std::string, User*>::iterator it=_server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
	{
		if (((*it).first).compare(channel_name) == 0 && ((*it).second->getNickname().compare(nickname) || (*it).second->getNickname().compare(_user->getNickname())) )
		{
				int fd = (*it).second->getFd();
				if (send(fd, a.c_str(), a.length(), 0) < 0)
					Log::printStringCol(CRITICAL, "ERROR: SENDING INVITE TO CHANNEL FAIELD.");
		}
	}



}


/* ======================================================================================== */
/* --------------------------------- QUIT COMMAND -----------------------------------  */
void Command::sendQuit(User* user)
{
	std::string channel_name;
	std::stringstream ss;
	for (size_t i=0; i < _server->_channel_users.size(); i++)
	{
		for(std::multimap<std::string, User*>::iterator it = _server->_channel_users.begin(); it != _server->_channel_users.end(); it++)
		{
			if (((*it).second->getNickname()).compare(_user->getNickname()) == 0)
			{
				channel_name = (*it).first;
				ss << _user->getNickUserHost() << " PART #" << channel_name << " ciao!" << "\r\n";
				 std::string a = ss.str();
				for(std::multimap<std::string, User*>::iterator it2 = _server->_channel_users.begin(); it2 != _server->_channel_users.end(); it2++)
				{
					if ((*it2).first.compare(channel_name) == 0)
					{
						int fd = (*it2).second->getFd();
						if (send(fd, a.c_str(), a.length(), 0) < 0)
							Log::printStringCol(CRITICAL, "ERROR: QUIT FAILED.");
					}
				}
				_server->_channel_users.erase(it);
				break ;
			}
		}
	}

	for(std::vector<Channel*>::iterator it = _server->_channels.begin(); it != _server->_channels.end(); it++)
	{
		if ((*it)->getName().compare(channel_name) == 0 )
			(*it)->deleteUser(_user);
	}

 	user->setState(DELETE);

}

/* ======================================================================================== */
/* -------------------------------- HELPER FUNCTIONS  ------------------------------------  */
std::string Command::put_reply_cmd(User* user, std::string err_num, std::string cmd, std::string code)
{
	std::stringstream ss;
	ss << err_num << " " << user->getNickname() << " " << cmd << " :" << code;
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
