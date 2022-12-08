#include "../includes/User.hpp"

User::User(int fd, uint16_t port)
	: _fd(fd), _port(port)
{
	// with or without @

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

void User::receiveData(Server *server)
{
	if (!server)
		return;

	size_t size = 0;

	// recv() is used to receive data from a socket. It returns the length of the message when successfully received
	// excess bytes may be discarded depending on the type of socket the message is received from.
	/* If no messages are available at the socket, the receive calls wait for a message to arrive, unless the socket is nonblocking
       (see fcntl(2)), in which case the value -1 is returned and errno is set to EAGAIN or EWOULDBLOCK.   */

	if (size == recv(this->_fd, &this->_buffer, BUFFER_SIZE, 0) < 0)				// search for NICK UND USER DANN REGISTER
	{
		//wenn user mehreren channels hinzutreten will dann am comma splitten (wenn JOIN #)
		log.printString(RED, "Error: No data received by user.");
		return ;
	}
	// could be eof/0 bytes received
	std::cout << this->_buffer << std::endl;

	if (size == 0)
	{

		// aus channel rausschmeißen		//channel kick (jeder user bekommt nachricht, und ganz rechts neue liste an usern (wie bei knuddels))
		log.printString(RED, "status == delete");		//status == Delete;	// delete user?
		// user logt sich aus und muss disconnected werden und dann gelöscht
	}

	// terminate buffer string
	buffer[size] = 0;

	// add incoming data to previously stored buffer var
	// this->_buffer += this->_buffer;
}

std::string User::getUsername() { return _username; }




// Channel: Nummeridentifikationen bei irc:
// 001 welcome
// 353 liste an usern die im channel connected sind
// 366 ende von der Liset
// 401 no such nick / channel


// message: /leave muss weg (wird angezeigt)