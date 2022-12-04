#include "User.hpp"

User::User(int fd, int port)
	: _fd(fd), _port(port)
{
	// with or without @
	this->_hostname = "@42-Queenz.fr.42";
	this->_state = true;
}

User::~User() {}

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

void User::receive()
{
	std::cout << "RECEIVE" << std::endl;
}

void User::registerNewUser()
{
	std::cout << "REGISTER NEW USER" << std::endl;
}

std::string User::getUsername() { return _username; }

