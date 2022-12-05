#include "../includes/User.hpp"

User::User(int fd, int port)
	: _fd(fd), _port(port)
{
	// with or without @
	this->_hostname = "@42-Queenz.fr.42";
	this->_state = true;
}

User::~User() {}
int User::getFd()	{ return this->_fd; }

// std::string setNickUserHost()
// {
// 	std::stringstream ss;

// 	ss << this->_nickname + "!" + this->_username + "@" + Server::hostname;

// 	return ss.str;
// }

// void User::setHostname()
// {
// 	this->_hostname = Server::getHostname();
// 	this->_hostname = gethostbyname();
// }

// std::string User::getHostname()
// {
// 	return Server::getHostname();


bool User::getState() { return _state; }

#define BUFFER_SIZE 520

void User::receiveData(Server *server)
{
	if (!server)
		return;
		
	size_t size = 0;
	char buffer[BUFFER_SIZE + 1];		//    BUFFER_SIZE = max msg length from Weechat

	// recv() is used to receive data from a socket. It returns the length of the message when successfully received
	// excess bytes may be discarded depending on the type of socket the message is received from.
	/* If no messages are available at the socket, the receive calls wait for a message to arrive, unless the socket is nonblocking
       (see fcntl(2)), in which case the value -1 is returned and errno is set to EAGAIN or EWOULDBLOCK.   */

	if (size == recv(this->getFd(), &buffer, BUFFER_SIZE, 0) < 0)
	{
		log.printString("Error: No data received by user.");
		return ;
	}
	// could be eof/0 bytes received
	if (size == 0)
	{
		log.printString("status == delete");		//status == Delete;	// delete user?
	}

	// terminate buffer string
	buffer[size] = 0;	

	// add incoming data to previously stored buffer var
	this->buffer += buffer;
}

std::string User::getUsername() { return _username; }

