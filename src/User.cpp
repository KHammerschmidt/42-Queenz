#include "../includes/User.hpp"
#include "../includes/Command.hpp"

User::~User() {}

// void NICK(Command* command)
// {
// 	if (!command)
// 		return ;

// 	std::string nickname = command->getParameters()[0];
// 	std::cout << nickname << std::endl;
// };

// void USER(Command* command);
// void PING(Command* command);
// void JOIN(Command* command);
// void PRIVMSG(Command* command);

User::User(int fd, sockaddr_in u_address, Server* server)
												:	_server(server), _hostname(HOSTNAME), 
													_hostaddr(inet_ntoa(server->getAddr().sin_addr)),
													_port(ntohs(server->getAddr().sin_port)), 
													_user_address(u_address), 
													_fd(fd),
													_last_ping(std::time(0)),
													_state(CONNECTED),
													_username(), _nickname(), _fullname(), 
													_nick_user_host(),
													_buffer(), _dataToSend(), 
													command_function()
{
	// command_function["NICK"] = NICK;
	// command_function["USER"] = USER;
	// command_function["PING"] = PING;
	// command_function["JOIN"] = JOIN;
	// command_function["PRIVMSG"] = PRIVMSG;
	// command_function["NOTICE"] = NOTICE;
}

int 		User::getFd() { return this->_fd; }
void 		User::setLastPing(time_t last_ping) { this->_last_ping = last_ping; }
void 		User::setNickUserHost() { this->_nick_user_host =  this->getNickname() + "!" + this->getUsername() + HOSTNAME; }
void 		User::setNickUserHost2(std::string output_to_client){this->_nick_user_host = output_to_client;};
void 		User::setState(int new_state) { this->_state = new_state; }
void 		User::setNickname(const std::string& nick){this->_nickname = nick; }
bool 		User::isRegistered() const { return (this->_state >= REGISTERED); }
int 		User::getState() { return this->_state; }
time_t 		User::getLastPing() const { return this->_last_ping; }
std::string	User::getUsername() { return this->_username; }
std::string User::getNickname() { return this->_nickname; }


/* Function gets called when there is data to receive for the user. */
void User::onUser(void)
{
	receiveMessage();
	splitMessage();
	invokeMessage();
}

/* User receives data with recv() and saves the read bytes within buffer string. */
void User::receiveMessage(void)
{
	char recv_buffer[BUFFER_SIZE + 3];
	memset(&recv_buffer, 0, sizeof(BUFFER_SIZE + 1));

	size_t size = recv(this->_fd, &recv_buffer, BUFFER_SIZE, 0);
	if (size < 0)					// search for NICK UND USER DANN REGISTER otherwise delete user
	{
		Log::printStringCol(WARNING, "No data received by user.");
		return ;
	}
	else if (size == 0)
	{
		// could be eof or 0 bytes received
		// aus channel rausschmeißen		//channel kick (jeder user bekommt nachricht, und ganz rechts neue liste an usern (wie bei knuddels))
		Log::printStringCol(CRITICAL, "status == delete");		//status == Delete;	// delete user?
		this->_state = DELETE; 									// user logt sich aus und muss disconnected werden und dann gelöscht
	}
	else
	{
		//if messages longer than BUFFER_SIZE they are being truncated.
		// if (size >= 510)
		// {
		// 	recv_buffer[511] = "\r";
		// 	recv_buffer[512] = "\n";
		// 	recv_buffer[513] = 0;
		// }
		// else
		recv_buffer[size] = 0;			
		this->_buffer.append(recv_buffer);
	}
}

/* Splits messages when separated by END_MSG ("\r\n"). */
void User::splitMessage(void)
{
	size_t pos;
	std::string delimiter(MSG_END);

	while ((pos = this->_buffer.find(delimiter)) != std::string::npos)
	{
		std::string tmp = this->_buffer.substr(0, pos);
		this->_buffer.erase(0, pos + delimiter.length());
		this->_dataToSend.push_back(tmp);
	}
}

void User::authNickname(void)
{
	std::cout << "Authentification Nickname" << std::endl;
	setState(NICKNAME);
}

void User::authUsername(void)
{
	std::cout << "Authentification Username" << std::endl;
	setState(USERNAME);
}
void User::authPassword(void)
{
	std::cout << "Authentification Password" << std::endl;
	setState(PASSWORD);

}

//missing deleting from dataToSend vector when dealt with
void User::invokeMessage(void)
{
	// std::vector<std::string> erase = std::vector<std::string>();

	for (std::vector<std::string>::iterator iter = _dataToSend.begin(); iter != _dataToSend.end(); iter++)
	{
		if (!isRegistered())
		{
			Log::printStringCol(CRITICAL, *iter);
			if ((*iter).find("/NICK") != std::string::npos)
				this->authNickname();
			else if ((*iter).find("/USER") != std::string::npos)
				this->authUsername();
			else if ((*iter).find("/PASS") != std::string::npos)
				this->authPassword();
			// _dataToSend.erase(*iter);
		}
		else if (this->getState() >= REGISTERED)
		{
			std::cout << "DEBUG: dataToSend " << *iter << std::endl;
			//Command* new_command = new Command(this, this->_server, *iter);
			//new_command->execute();
			this->command_function.push_back(new Command(this, this->_server, *iter));
		}
		else
		{
			std::cout << "DELTE USER" << std::endl;
		}
	}

}
// 	for (std::vector<std::string>::iterator iter = command_function.begin(); iter != command_function.end(); iter++)
// 	{
// 		iter->execute();
// 	}


		// switch(extract_command(*iter))	//*iter is a string
		// {
		// 	case NICK:
		// 		// sendNickname(this, *iter);
		// 		break ;
		// 	case USER:
		// 		// sendUsername(this, *iter);
		// 		break ;
		// 	case PING:
		// 		// sendPong(this, *iter);
		// 		break ;
		// 	case JOIN:
		// 		// sendJoin(this, *iter);
		// 		break ;
		// 	case PRIVMSG_CH:
		// 		// sendPrivMsgChannel(this, *iter);
		// 		break ;
		// 	case PRIVMSG_U:
		// 		// sendPrivMsgUser(this, *iter);
		// 		break ;
		// 	default:
		// 		Log::printStringCol(CRITICAL, "ERROR: COMMAND UNKNOWN");
		// 		// user->_buffer.erase(0, user->_buffer.length());
		// 		return ;
		// 	break ;
		// // }	
		// this->_command_function.push_back(new Command(this, this->_server, tmp));
	// }
// }


// int User::extract_command(const std::string& message)
// {
// 	if (message.find("/NICK") != std::string::npos)
// 		return 0;
// 	else if (message.find("/USER") != std::string::npos)
// 		return 1;
// 	else if (message.find("/PING") != std::string::npos)
// 		return 2;
// 	else if (message.find("/JOIN") != std::string::npos)
// 		return 3;
// 	else if (message.find("/PRIVMSG #") != std::string::npos)
// 		return 4;
// 	else if (message.find("/PRIVMSG") != std::string::npos || message.find("/NOTICE") != std::string::npos)
// 		return 5;
// 	else
// 		return -1;
// }

void User::write(const std::string& message) const
{
	std::string str = message + static_cast<std::string>(MSG_END);
	if (send(this->_fd, str.c_str(), str.length(), 0) < 0)
		Log::printStringCol(CRITICAL, "ERROR: SENDING MESSAGE FROM USER FAILED.");
}



//username <= 9 characters


// Channel: Nummeridentifikationen bei irc:
// 001 welcome
// 353 liste an usern die im channel connected sind
// 366 ende von der Liset
// 401 no such nick / channel


// message: /leave muss weg (wird angezeigt)




// register user
// NICK - register nickname
// USER - register Username
// print RPL_WELCOME after NICK and USER have been received and processed



// command PING params: <server1> <server2>
// test presence of an active client or server at the other end
// of connection
// server sends regular intervals a ping when no other
// activity detected from that connection





//wenn user mehreren channels hinzutreten will dann am comma splitten (wenn JOIN #)


void User::sendPong()
{
	// this->write()
	// wenn keine Nachricht dann schickt user ping (nach einer Minute oder so) und server muss an den user poing senden ansonsten "connection lost"
}

void reply(std::string& reply)
{
	if (reply.length() != 0)
		std::cout << "Reply function here";
}

void join(Channel* channel)
{
	if (channel)
		std::cout << "join channel function here";
}



//other commands
// command_function["PASS"] = PASS;
// command_function["PONG"] = PONG;

// command_function["OPER"] = OPER;
// command_function["MODE"] = MODE;
// command_function["INVITE"] = INVITE;
// command_function["KICK"] = KICK;
// command_function["QUIT"] = QUIT;

// command_function["PART"] = PART;
// command_function["TOPIC"] = TOPIC;
// command_function["KILL"] = KILL;

// command_function["CONNECT"] = CONNECT;
// command_function["AWAY"] = AWAY;
