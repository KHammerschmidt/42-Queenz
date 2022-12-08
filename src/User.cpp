#include "../includes/User.hpp"

User::~User() {}
User::User(int fd, uint16_t port)
	: _fd(fd), _port(port), _last_ping(std::time(0)), _dataToSend()
{
	// with or without @
	this->_state = CONNECTED;
}

bool User::isRegistered() const { return this->_state >= REGISTERED; }

std::string	User::getUsername() { return this->_username; }
std::string User::getNickname() { return this->_nickname; }
bool 		User::getState() { return _state; }
int 		User::getFd() { return this->_fd; }
time_t 		User::getLastPing() const { return this->_last_ping; }
void 		User::setLastPing(time_t last_ping) { this->_last_ping = last_ping; }
void 		User::setNickUserHost() { this->_nick_user_host =  this->getNickname() + "!" + this->getUsername() + HOSTNAME; }

/* User receives data with recv() and saves the read bytes within private this->_buffer string. */
void User::receiveData()
{
	char recv_buffer[BUFFER_SIZE + 1];
	memset(&recv_buffer, 0, sizeof(BUFFER_SIZE + 1));

	size_t size = recv(this->_fd, &recv_buffer, BUFFER_SIZE, 0);
	if (size < 0)	// search for NICK UND USER DANN REGISTER
	{
		std::cout << "BUFFER: " << recv_buffer << std::endl;
		Log::printStringCol(WARNING, "No data received by user.");
		return ;
	}
	else if (size == 0)
	{
		// could be eof or 0 bytes received
		// aus channel rausschmeißen		//channel kick (jeder user bekommt nachricht, und ganz rechts neue liste an usern (wie bei knuddels))
		Log::printStringCol(CRITICAL, "status == delete");		//status == Delete;	// delete user?
		// user logt sich aus und muss disconnected werden und dann gelöscht
	}
	else
	{
		buffer[size] = 0;
		// previous received messaged are being appended to member variable _buffer.
		this->_buffer.append(recv_buffer);

		Log::printStringCol(LOG, recv_buffer);
		Log::printStringCol(LOG, this->_buffer);
	}
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


void User::write(std::string msg)
{
	this->_dataToSend.push_back(msg);
}



//wenn user mehreren channels hinzutreten will dann am comma splitten (wenn JOIN #)


void User::sendPong()
{

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