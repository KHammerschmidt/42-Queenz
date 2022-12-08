#include "../includes/Command.hpp"

/* Default constructor which takes the server, the user which entered a command and the received message. */
Command::Command(Server *server, User* user, const std::string& msg) 
	: _server(server) { execute(user, msg); }

/* Command destructor destroys all Command classes it invoked. */
Command::~Command() {}

/* Could also splits the msg and puts content into vector fields for printing. ??? */
/* Extracts the portion of the string that specifies the command to be performed. */
int Command::extractCommand(const std::string& msg)
{
	if (msg.length() == 0)
		return -1;
	
	std::cout << "Calling sendPrivMsgUser" << std::endl;
	this->_server->getStatus();
	return 1;
}

/* Executes the user's command. */
void Command::execute(User* user, const std::string& msg)
{
	switch(extractCommand(msg))
	{
		case PRIVMSGCH:
			sendPrivMsgChannel(user, msg);
			break ;
		case PRIVMSGU:
			sendPrivMsgUser(user, msg);
			break ;
		case PING:
			sendPing(user, msg);
			break ;
		default:
			std::cout << "Error: unknown command" << std::endl;
			return ;
	}
}

/* Sends a private message to all channel members. */
void Command::sendPrivMsgChannel(User* user, const std::string& msg)
{
	if (!user || msg.length() == 0)
		return ;
	std::cout << "Send here private message to channel members" << std::endl;
}

/* Sends a private message to a user. */
void Command::sendPrivMsgUser(User* user, const std::string& msg)
{
	if (!user || msg.length() == 0)
		return ;
	std::cout << "Send here private message to a user" << std::endl;
}

/* Sends a ping to the server. */
void Command::sendPing(User* user, const std::string& msg)
{
	if (!user || msg.length() == 0)
		return ;
	std::cout << "Send a ping to server" << std::endl;
}















// void send_privmsg_priv() { return; }
// void send_privmsg_channel() { return; }
// void invalid_cmd() { return; }
