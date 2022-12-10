#include "../includes/Server.hpp"

int 		Server::getPort() const { return this->_port; }
int 		Server::getTimeout() const { return this->_timeout; }
bool 		Server::getServerStatus() const { return this->_serverRunningStatus; }
std::string Server::getPassword() const { return this->_password; }

Server::~Server()
{
	// delete channels // more to add??? ????
	Log::printStringCol(REGULAR, "Disconnecting...bye bye...");

	for (std::map<int, User*>::iterator iter = this->_users.begin(); iter != this->_users.end(); iter++)
	{
		Log::printStringCol(LOG, "DELETING / DISCONNECTING USER");
		// disconnectUser(*(*iter)->second);
		// deleteUser(*iter->first);  // implement
		// deleteUser(*iter->second);
	}
	delete this;
}

Server::Server(char** argv)	//wenn static instance dann constructor ohne parameter
{
	Log::printStringCol(REGULAR, WELCOME_BEFORE);

	setPort(argv[1]);
	this->_password = argv[2];

	newSocket();
	this->_timeout = 3 * 60 * 1000;

	pollfd server_fd = { this->_sockfd, POLLIN, 0};
	memset(&_pollfds, 0, sizeof(this->_pollfds));
	this->_pollfds.push_back(server_fd);
}

void Server::setServerStatus(bool new_status) { this->_serverRunningStatus = new_status; }
void Server::setPort(std::string port_str)
{
	this->_port = std::atoi(port_str.c_str());
	setServerStatus(true);
}

/* Creates a new socket, sets options and binds it to the server and listens on socket. */
void Server::newSocket(void)
{
	this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sockfd < 0)
		serverError(1);

	int enable = 1;
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) != 0)
		serverError(1);

	if (fcntl(this->_sockfd, F_SETFL, O_NONBLOCK) < 0)
		serverError(1);

	struct sockaddr_in serv_address;
	memset(&serv_address, 0, sizeof(serv_address));
	serv_address.sin_family = AF_INET;
	serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_address.sin_port = htons(this->_port);

	if (bind(this->_sockfd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0)
		serverError(1);

	if ((listen(this->_sockfd, serv_address.sin_port) < 0))
		serverError(1);
}

/* Prints error messages, closes server fd and sets server status to false which shuts down the server. */
void Server::serverError(int code)
{
	switch(code)
	{
		case 1: Log::printStringCol(CRITICAL, ERR_SOCKET_CONNECTION); break;
		case 2: Log::printStringCol(CRITICAL, ERR_POLL); break;
		case 3: Log::printStringCol(CRITICAL, ERR_ACCEPT); break;
		case 4: Log::printStringCol(CRITICAL, ERR_USER_CREDENTIALS); break;
		case 5: Log::printStringCol(CRITICAL, ERR_PING_TIMEOUT); break;
		case 6: Log::printStringCol(CRITICAL, ERR_SEND); break;
		case 7: Log::printStringCol(WARNING, ERR_POLLIN); break;
		default: Log::printStringCol(CRITICAL, ERR_ANY_OTHER_ERR); break;
	}

	close(this->_sockfd);
	setServerStatus(false);
	exit(-1);
}

/* Makes the server listen to incoming requests and loops over these requests to identify a POLLIN event.
   If file descriptor is readable, the pollfds struct indicates if either a new user registered or another
   command has been sent. The function then calls the respective function. */
void Server::run()
{
	while (this->getServerStatus() == true)
	{
		Log::printStringCol(REGULAR, SERV_LISTENING);

		if (poll(_pollfds.begin().base(), _pollfds.size(), this->_timeout) < 0)
			serverError(2);

		for (pfds_iterator = this->_pollfds.begin(); pfds_iterator != this->_pollfds.end(); pfds_iterator++)
		{
			if (pfds_iterator->revents == 0)
				continue;

			//who sends ping? to where? and the same than revents == 0?

			if ((pfds_iterator->revents & POLLHUP) == POLLHUP)
				serverError(7);

			if ((pfds_iterator->revents & POLLIN) == POLLIN)
			{
				if (this->_pollfds[0].revents == POLLIN)
				{
					connectNewUser();
					break ;
				}
				this->_users[pfds_iterator->fd]->readMessage(this);
			}
		}
	}
}

User* Server::getUser(int fd)
{
	for (std::map<int, User*>::iterator iter = this->_users.begin(); iter != this->_users.end(); iter++)
	{
		if ((*iter).first == fd)
			return ((*iter).second);
	}
	return NULL;
}

/* To register, user sends request via server socket, the only outer connection. A new fd for user is created with accept(). */
void Server::connectNewUser()
{
	int new_fd;
	struct sockaddr_in s_address;
	socklen_t s_size = sizeof(s_address);

	new_fd = accept(this->_sockfd,(sockaddr *) &s_address, &s_size);
	if (new_fd < 0)
		serverError(3);
	else
		Log::printStringCol(LOG, NEW_CONNECTION);

	pollfd user_pollfd = {new_fd, POLLIN, 0};
	this->_pollfds.push_back(user_pollfd);

	User* new_user = new User(new_fd, ntohs(s_address.sin_port));
	if (new_user->getState() != 0)
	{
		Log::printStringCol(REGULAR, ERR_USER_REGISTRY);
		//deleteUser(new_user);	//check with test server what happens
		return ;
	}

	this->_users.insert(std::make_pair(new_fd, new_user));
	std::cout << "New User on port: new_fd: " << new_fd << " | inet_ntoa: " << inet_ntoa(s_address.sin_addr) << ":" << ntohs(s_address.sin_port) << " (" << new_fd << ")" << std::endl;
}

/* Server sends Ping to users to see if connection is still intact. */
void Server::sendPing()
{
	time_t current_time = std::time(0);

	for (std::map<int, User*>::iterator iter = _users.begin(); iter != _users.end(); iter++)
	{
		if (current_time - ((*iter).second->getLastPing()) >= static_cast<int>(this->_timeout))
			serverError(5);																//disconnect user / delete user / etc.
		else if ((*iter).second->getState() == true)									//online
			(*iter).second->write("PING " + (*iter).second->getNickname());				//user has then to answer with pong to server
	}
}

/* Erases file descriptor from vector _users and _pollfds.
	Maybe also delete user from channel? how to check internal? */
void Server::disconnectUser(User* user)
{
	if (user)
		Log::printStringCol(LOG, "DISCONNECTING USER");

	// // maybe in try / catch in case of fd cannot be find/invalid fd?
	// // User* user = this->_users.at(fd);
	// int user_fd = getUserfd(user);
	// if (user_fd < 0)
	// {
	// 	Log::printStringCol(RED, "Error: no user found.");
	// 	serverError(4);
	// }

	// user->leave();		// leave channels
	// // erase this user from std::map by its fd
	// this->_users.erase(user_fd);

	// // erase user from pollfds
	// for (pfds_iterator = _pollfds.begin(); pfds_iterator != _pollfds.end(); pfds_iterator++)
	// {
	// 	if (pfds_iterator->fd == user_fd)
	// 	{
	// 		this->_pollfds.erase(pfds_iterator);
	// 		close(user_fd);
	// 		break ;
	// 	}
	// }
	// delete user;
	// deleteUser();
}


/* Function returns a vector with User objects, extracted from member type std::map<int, User*>. */
// std::vector<User*> Server::getUsers() const
// {
// 	std::vector<User *> users = std::vector<User*>();

// 	for (std::map<int, User *>::iterator iter = this->_users.begin(); iter != this->_users.end(); iter++)
// 		users.push_back(iter->second);

// 	return users;
// }

// int Server::getUserfd(User* user)
// {
// 	int fd;
// 	for (std::map<int, User*>::iterator iter = this->_users.begin(); iter != this->_users.end(); iter++)
// 	{
// 		if (*iter->second == user)
// 			return user->getFd();
// 	}
// 	return -1;
// }

/* Function prints User credentials: nickname, username, fullname fd */
// void Server::printUser()
// {
// 	for (std::map<int, User*>::iterator iter = users.begin(); iter != users.end(); iter++)
// 	{
// 		std::cout << "nickname: " << iter.getNickname() << " | username: " << iter->getUsername() << " | fullname: " << iter->getFullName();
// 		std::cout << " | port: " << iter.getFD() << std::endl;
// 	}
// }






/* This structure is filled in with the address of the peer socket, as
known to the communications layer. The exact format of the address returned addr is determined by the sockets address
family (see socket(2) and the respective protocol man pages). When addr is NULL, nothing is filled in; in this case, addrlen is
not used, and should also be NULL. */



// if (pfds_iterator->revents != POLLIN)		//e.g. POLLHUP (in explanations.txt) when POLLHUP then disconnect and break
// {
// 	setServerStatus(false);
// 	Log::printStringCol(RED, "Error: unexpected result. Nothing to read. Connection will be disabled.");
// 	break ;
// }


///jeder user hat einen msg string bei dem die neue Nachricht appended wird (
// \r\n heisst ende der Nachricht danach



// ip vom server


// HOW TO CONNECT SO WEECHAT, du brauchst 2 Terminals (cd 42-Queenz)
// Terminal 1: $ weechat
// 			$ /server add irc local-ip/port
// Terminal 2: $ make re
// 			$ ./ircserv port pw (ich habs immer auf port 8080 gemacht)
// Terminal 1: $ /connect irc -password=pw
// 			$ /connect irc (um User zu registrieren)
// Dann bekommst du angezeigt dass der neue User registriert wurde aber keine Daten eingelesen wurden.


// link server client
// /server add irc(servername) interne ip 10.11.27/420
// dann terminal
// nc -l 420
// /connect irc -password=pw
// mit nc -C und localer ip addresse einen neuen user connecten(?)






















	// Max:
	// client ist ein pair client <pollfd, User*> keine Map
	// ip rausholen () --> als integer kommt der und der muss umgewandelt werden in (1. byte )
	// int umwandeln zum string ip addresse (1. byte rechts ist die linke Zahl bei IP addresse)
	// create a new User with


// NOT SURE ABOUT LOOP
// OLD VERSION:
// if (this->pfds_iterator->revents == POLLIN)
// {
// 	// when POLLIN on other pollfd.fd than [0] it means a command (PRIVMSG, etc..) has been send.
// 	for (std::vector<pollfd>::iterator iter_poll = _pollfds.begin(); iter_poll != _pollfds.end(); iter_poll++)
// 	{
// 		if (iter_poll->revents == POLLIN)
// 			this->_users[iter_poll->fd]->receiveData();		// receive(this)	// implement receive Funktion in User
// 	}
// }

// // _pollfds[0] stellt das erste pollfd struct dar in dem die server socket gespeichert ist. Nur über diese socket können sich neue User registrieren.
// if ((this->_pollfds[0].revents & POLLIN) == POLLIN)		// oder (this->_pollfds... & POLLIN)
// {
// 	Log::printStringCol(LOG, "A new user is being connected to server");
// 	connectNewUser();
// 	Log::printStringCol(LOG, "--- Done. User connected to server");
// 	break ;
// }

// if ((this->pfds_iterator->revents & POLLIN) == POLLIN)
// {
// 	// // when POLLIN on other pollfd.fd than [0] it means a command (PRIVMSG, etc..) has been send.
// 	// for (std::vector<pollfd>::iterator iter_poll = _pollfds.begin(); iter_poll != _pollfds.end(); iter_poll++)
// 	// {
// 	// 	if (iter_poll->revents == POLLIN)
// 	this->_users[iter_poll->fd]->receiveData(this);		// receive(this)	// implement receive Funktion in User
// 	// }
// 	// break ;
// 	Log::printStringCol(LOG, "User receive Data loop --- end");
// }
// else
// {
// 	// wenn kein POLLIN dann error Nachricht und rausbrechen
// 	std::cout << " (this->pfds_iterator->revents != POLLIN)" << std::endl;
// 	std::cout << "error msg and disconnect Server" << std::endl;
// 	this->setServerStatus(false);
// 	break ;
// }








// PING
// if (std::time(0) - last_ping >= ping)

// if (getUser(pfds_iterator->fd).getLastPing() + >= this->_timeout)
// 	this->sendPing();
// 	this->_users[pfds_iterator->fd]->send
// 	command->getUser().sendTo(command->getUser(), "PONG :" + command->getParameters()[0]);
