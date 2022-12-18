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
	: _user(user), _server(server), query(message), authenticated(false), command_state(false),	reply_state(false), user_command(""), channels_replies()
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
		sendJoin(_user, query, _server);
	else if (this->user_command == "PING")
		sendQuit(_user);
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
	this->_user->setNickUserHost();//needed here ^^

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

/*NOTICE behaviour:
like PRIVMSG,but:
if query not opened, 
	print message in server;
otherwise p
	rint it in query.
*/
void Command::sendPrivMsgUser(User* user, std::string msg)		
{				
    int index_of_first_space;

	index_of_first_space = msg.find_first_of(" ");
	std::string command = msg.substr(0, index_of_first_space);
	std::string command_arg = msg.substr(index_of_first_space + 1, msg.length() - index_of_first_space);
	if (command.compare("PRIVMSG") != 0 && command.compare("NOTICE") != 0)
		return ;//write error and return 
	
	//find  first space to have lenght of nick
	index_of_first_space = command_arg.find_first_of(" ");
	if (!index_of_first_space)
		return ;
	std::string nick_receiver = command_arg.substr(0, index_of_first_space);
	//text to print
   	std::string text = command_arg.substr(index_of_first_space + 1, command_arg.length() - index_of_first_space);
		std::cout << "------" << "1" << "++++++++\n";

	//check if is a Channel related messaged
	if (nick_receiver.substr(0, 1).compare("#") == 0)
	{
			std::cout << "------" << "2" << "++++++++\n";

		//check if channel name is valid (voraussetzt get_users function was called and channels->replied is not empty)
		if (this->channels_replies.size() > 0)
		{				sendChannelMsg(user, text, nick_receiver);
		std::cout << "------" << "2.5--size= " << this->channels_replies.size() << "++++++++\n";
		}
		//else
			//print error, channel doesnt exist
		return ;
	}

	if (return_user_in_server(nick_receiver) == NULL)
		return ;



	std::stringstream ss;
	this->command_state = true;
	//std::cout << user->getNickUserHost() << "-------";//test
	ss << user->getNickUserHost() << " " << command << " " << nick_receiver << " " << text << "\r\n";
	this->_command_message = ss.str();
	this->receiver_fd = return_user_in_server(nick_receiver)->getFd();

	Log::printStringCol(CRITICAL, msg);

};


//not more needed.
// Channel	*Command::return_channel_in_server(const std::string channel_name, Server *server)
// {
// 	std::vector<Channel*> channel_temp = server->_channels;

// 	for (std::vector<Channel*>::iterator it = channel_temp.begin(); it != channel_temp.end(); it++)
// 	{
// 		if ((*it)->getName() == channel_name)
// 			return (*it);
// 	} 
// 	return NULL;	//throw error
// }

//create a string with all nicknames of users of this channel
std::string Command::return_string_all_users_in_channel(const std::string channel_name, Server *server, User *user)
{//353
	std::multimap<std::string, User*> map_temp = server->_channel_users;
	std::multimap<std::string, User*>::iterator it;
	std::string ss;


	for (;;)
	{		
		it = map_temp.find(channel_name);
		if (it !=  map_temp.end() && it->second->getNickname() != user->getNickname())
		{
			//std::cout << "+++++" << (*it).second->getNickname() << "------" << nickname << "+++++\n";
			ss.append((*it).second->getNickname());
			ss.append(" ");
			map_temp.erase(it);
			continue;
		}
		break;
	}
	if (!ss.empty())
		ss.erase(ss.length() -1);
	return (ss);
}



//create a vector with all the users enrolled in this channel. Also check if user already in channel and returns a boolean false
bool Command::get_users_in_channel(const std::string channel_name, Server *server, User *user)
{
	std::multimap<std::string, User*> map_temp = server->_channel_users;
	std::multimap<std::string, User*>::iterator it;
	int i = 0;
	
	if (map_temp.size() == 0)
	{
		this->channels_replies.push_back(user);
		return true;
	}
	//add condition: spring just if actual user is in map of channel_users && channelname already exist in channels
	if ((map_temp.find(channel_name))->second->getNickname() == user->getNickname() && std::find(server->_channels_by_name.begin(), server->_channels_by_name.end(), channel_name ) != server->_channels_by_name.end()) 
		return false;

	for (;;)
	{		
		it = map_temp.find(channel_name);
		if (it !=  map_temp.end())
		{
			//std::cout << "------" << "2.5.2--USER= " << map_temp.find(channel_name)->second << "++++++++\n";
			//if ((map_temp.find(channel_name)->second)->getNickname().compare(nickname) != 0)
			this->channels_replies.push_back(user);
			map_temp.erase(it);
			if (i++ > 20)
				break;
		}
		break;
	}
	return true;
	//write in another way
	// IDEE-> but bad idea working with original channel->users
	// for (;;)
	// {		
	// 	it = server->_channel_users.find(channel_name);
	// 	if (it !=  server->_channel_users.end())
	// 	{
	// 		if ((server->_channel_users.find(channel_name)->second)->getNickname().compare(nickname) != 0)
	// 			this->channels_replies.push_back(server->_channel_users.find(channel_name)->second);
	// 		if (i++ == )
	// 			break;
	// 		//std::cout << "------" << "2.5.--USER= " << server->_channel_users.find(channel_name)->second << "++++++++\n";
	// 		//server->_channel_users.erase(it);
	// 		continue;
	// 	}
	// 	break;
	// }



}


//in server add a vector with all channels names or in user all the joined channels; so I can test here if need to create a new one or not, without creating a temp channel to check it(line 29 channel)
void Command::sendJoin(User* user, const std::string msg, Server* server)
{
    int index_of_first_space;

	index_of_first_space = msg.find_first_of(" ");
	std::string command = msg.substr(0, index_of_first_space);
	std::string channel_name = msg.substr(index_of_first_space + 2, msg.length() - index_of_first_space);
	std::string prefix_channel = msg.substr(index_of_first_space + 1, 1);//msg.length() - index_of_first_space - channel_name.length() - 1);
	

	//test command is JOIN
	if (command.compare("JOIN") != 0 || prefix_channel.compare("#") !=0)// "&#!+" -> should we handle them?
		return ;//write error and return 


	//test channel_name errors	
	if (channel_name.find(" ") != (unsigned long) -1 || channel_name.find(":") != (unsigned long) -1 
														 || channel_name.find(",") != (unsigned long) -1)
			return; //print error invalid channel_name
	
	//test channel_name lenght
	if (channel_name.length() > 50)
    	    channel_name.resize(50);
	
	






	//test if channel aready exist	
   //return vector with all users in channel in server (can use just this actually)
	if (get_users_in_channel(channel_name, server, user) == false)
		return; //print error: User already present in channel
	//std::cout << "------" << "2.5.2.0--size= " << this->channels_replies.size() << "++++++++\n";

	if (this->channels_replies.size() == 1)//means vector was not filled-> no user setted for the channel->  channel doesnt exist
	{
		Channel* new_channel = new Channel(channel_name);
		server->_channels.push_back(new_channel);
		server->_channels_by_name.push_back(new_channel->getName());
	}
	// add current user to channel map		
	server->_channel_users.insert(std::pair<std::string, User*> (channel_name, user));// statt new channel find the already existing channel->getName()

	/*check numbers of Chicago by JOIN Replies Command*/
	this->reply_state = true;
	this->command_state = true;
	//2)Reply to all users in channel + reply to sender user; 
	//all receives: UserNickHostSender << "JOIN" << " #" << Channel_name; 
	std::stringstream ss;
	ss << user->getNickUserHost() << " " << command << " #" << channel_name << "\r\n";
	this->_command_message = ss.str();
	//this->receiver_fd = return_channel_in_server(nick_receiver)->getFd();fix


	// this->reply_state = true;
	// this->command_state = false;
	//ALL USERS of Channel "channelname" in vector this->channels replies; here replies (2a, 2b, 2c)
	for (std::vector<User*>::iterator it = channels_replies.begin(); it != channels_replies.end(); it++)
	{

		this->receiver_fd = (return_user_in_server((*it)->getNickname())->getFd());

		//3) Reply RPL_TOPIC to sender: "bar.example.org"(what is this?) << " 332 " << nick_sender << " #" << channel_name << " :" << channel.getTopic(); 
		ss << "@" << HOSTNAME << " 332 " << user->getNickname() << " #" << channel_name << " :A timey wimey channel (this should be channelName->getTopic()" << "\r\n";
		this->_command_message = ss.str();
	
	
		//4a) RPL_NAMREPLY, users currently in channel: "bar.example.org"(what is this?) << " 353 " << nick_sender << " #" << channel_name << " :" << @user1(@means is an op) << " " << user2 << " " user_sender 
		ss << "@" << HOSTNAME << " 353 " << user->getNickname() << " #" << channel_name << " " << return_string_all_users_in_channel(channel_name, server, user) << "\r\n";
		this->_command_message = ss.str();
		//here some bug, check it
		//std::cout << "@" << HOSTNAME << " 353 " << user->getNickname() << " #" << channel_name << " " << return_string_all_users_in_channel(channel_name, server, user) << std::endl;
		
		//4b) RPL_ENDOFNAMES: "bar.example.org" << " 366 " << nick_sender << " #" << channel_name << " :End of NAMES list"
		ss << "@" << HOSTNAME << " 366 " << user->getNickname() << " #" << channel_name << " End of /NAMES list" << "\r\n";
		this->_command_message = ss.str();

	}
	//std::cout <<  "-------"  <<"++++++++++";
	//std::cout << "------" << "2.5.5--ERROR2" << "++++++++\n";

	Log::printStringCol(CRITICAL, msg);

};

//	formatierung: /msg #channel_name message
void Command::sendChannelMsg(User* user, std::string text, std::string channel_name)
{
			std::cout << "------" << "3" << "++++++++\n";

	std::stringstream ss;
	this->command_state = true;
	//std::cout << user->getNickUserHost() << "-------";//test
	ss << user->getNickUserHost() << " " << "PRIVMSG" << " #" << channel_name << " " << text << "\r\n";
	this->_command_message = ss.str();
	// for (std::multimap<std::string, User*>::iterator it = _server->_channel_users.begin(); it !=  _server->_channel_users.end(); it++)
	// {
	// 	this->receiver_fd = return_user_in_server((*it)->getNickname())->getFd();//not sure is right function
	// 	user->write();
	// }
	std::multimap<std::string, User*> map_temp = _server->_channel_users;
	std::multimap<std::string, User*>::iterator it;
		std::cout << "------" << "4" << "++++++++\n";

	for (;;)
	{					
		std::cout << "------" << "5" << "++++++++\n";
		it = map_temp.find(channel_name);
		if (it !=  map_temp.end())
		{
			std::cout << "------" << "++++++++\n";
			this->receiver_fd = return_user_in_server((*it).second->getNickname())->getFd();
			user->write();
			map_temp.erase(it);
			continue;
		}
		break;
	}
	this->command_state = false;
	this->reply_state = false;

	//Log::printStringCol(CRITICAL, text);
}


void Command::sendQuit(User* user)
{
	close(user->getFd());
	//close the socket but let the fd still on listening modus
	//https://stackoverflow.com/questions/27798419/closing-client-socket-and-keeping-server-socket-active
	//Log::printStringCol(CRITICAL, message);
};
