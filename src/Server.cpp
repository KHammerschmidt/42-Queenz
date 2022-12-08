#include "../includes/Server.hpp"

int Server::getSocket() const { return this->_sockfd; }
int Server::getPort() const { return this->_port; }
int Server::getTimeout() const { return this->_timeout; }
bool Server::getStatus() const { return this->_serverRunningStatus; }
std::string Server::getPassword() const { return this->_password; }
// std::string Server::getHostname() const { return this->_hostname; }

Server::~Server()
{
	log.printStringCol(REGULAR, "Disconnecting... bye bye");

	for (std::map<int, User*>::iterator iter = this->_users.begin(); iter != this->_users.end(); iter++)
	{
		log.printStringCol(LOG, "DELETING / DISCONNECTING USER");
		// disconnectUser(*(*iter)->second);
		// deleteUser(*iter->first);  // implement
		// deleteUser(*iter->second);
	}

}	// delete channels // more to add???

Server::Server(char** argv)
{
	log.printStringCol(REGULAR, "Welcome to our server 42-Queenz...");

	setPort(argv[1]);
	this->_password = argv[2];
	// this->_hostname = "42-Queenz.42.fr";

	newSocket();
	this->_timeout = 3 * 60 * 1000;
	// this->_error = -1;

	pollfd server_fd = { this->_sockfd, POLLIN, 0};
	memset(&_pollfds, 0, sizeof(this->_pollfds));
	this->_pollfds.push_back(server_fd);

	// this->_online;
	// this->_max_online;
}

void Server::setServerStatus(bool new_status) { this->_serverRunningStatus = new_status; }
bool Server::getServerStatus() const { return this->_serverRunningStatus; }

/* Checks for valid port and sets server status to running if input is valid. */
void Server::setPort(std::string port_str)
{
	int port = std::atoi(port_str.c_str());

	if (isdigit(port))
		serverError(0);

	this->_port = port;
	setServerStatus(true);
}

/* Creates a new socket, sets options and binds it to the server. 									*/
/* In detail: New socket is created and set to NON_BLOCKING. The sockaddr_struct is initialised
   (family = IPv4, open to all incoming connections and the respective port is logged). Socket is
   then linked to serv_address struct which enables the access to variables like address. The last
   point makes the created socket listen to the server port. 										*/
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
	memset(&serv_address, 0, sizeof(serv_address));		//or: bzero((char *) &serv_address, sizeof(serv_address));
	serv_address.sin_family = AF_INET;
	serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_address.sin_port = htons(this->_port);

	if (bind(this->_sockfd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0)
		serverError(1);

	if ((listen(this->_sockfd, serv_address.sin_port) < 0))
		serverError(1);
}

/* Prints an error message based on error code. Closes the server socket and sets server status
to false, hence stops the poll loop and makes the server shut down. 							*/
void Server::serverError(int code)
{
	// implement enum { NO_ERROR = -1, INPUT_PARAM = 0; ERROR_SOCKET_CONNECTION = 2, ERROR_POLL = 3, ERROR_ACCEPT = 4, ANOTHER_ERROR = 5}
	if (code == -1)
		return ;
	else if (code == 0)
		log.printStringCol(CRITICAL, "Error: Invalid input paramter");
	else if (code == 1)
		log.printStringCol(CRITICAL, "Error: The socket connection of server 42-Queenz.fr could not be established.");
	else if (code == 2)
		log.printStringCol(CRITICAL, "Error: while polling from sockfd");
	else if (code == 3)
		log.printStringCol(CRITICAL, "Error: accept() failed");
	else if (code == 4)
		log.printStringCol(CRITICAL, "Error: no user found by these credentials.");
	else if (code == 5)
		log.printStringCol(WARNING, "WARNING: ping timed out");
	else if (code == 6)
		log.printStringCol(CRITICAL, "Error: send() failed.");
	else
		log.printStringCol(CRITICAL, "Another error.");

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
		log.printStringCol(REGULAR, "Server is listening...");
		// einen User registrieren geht, wenn dann nochmal connect error

		// timeout: -1 infinite timeout // standard von IBM ist 3 Minuten
		if (poll(_pollfds.begin().base(), _pollfds.size(), this->_timeout) < 0)
			serverError(2);

		// NOT SURE ABOUT LOOP... if on top or around receiveData (has to be tested!)
		// loop through vector pollfds that lists all file descriptors on open connections
		for (_pfds_iterator = this->_pollfds.begin(); _pfds_iterator != this->_pollfds.end(); _pfds_iterator++)
		{
			// this means the file descriptor is not yet ready to be read,
			if (_pfds_iterator->revents == 0)
				continue;

			if ((_pfds_iterator->revents & POLLHUP) == POLLHUP)
			{
				log.printStringCol(WARNING, "NO POLLIN, server is shutting down");
				this->setServerStatus(false);
				break ;
			}

			if ((_pfds_iterator->revents & POLLIN) == POLLIN)
			{
				if (this->_pollfds[0].revents == POLLIN)
				{
					connectNewUser();
					break ;
				}

				this->_users[_pfds_iterator->fd]->receiveData();	
			}

			// wenn keine Nachricht dann schickt user ping (nach einer Minute oder so) und server muss an den user poing senden ansonsten "connection lost"
		}
	}
}

/* A new user sends a request via the server socket, which is the only socket client requests can come in.
   In order to process any further POLLIN requests, a new listening file descriptor is created via accept()
   and its content saved in struct sockaddr_in. A new user is created and stored in map _users <fd, User*>.
   User credentials are printed (for testing) and new pollfd struct is added to _pollfds vector.			*/
void Server::connectNewUser()
{
	int new_fd;
	struct sockaddr_in s_address;
	socklen_t s_size = sizeof(s_address);

	// ?? while im loop bis accept -1 returned
	// accept the incoming connetion; accept() returns a new fd that displays the connection
	new_fd = accept(this->_sockfd,(sockaddr *) &s_address, &s_size);
	if (new_fd < 0)
	{
		std::cout << "this is the errno number: " << errno << std::endl;
		serverError(3);
	}
	else
		log.printStringCol(LOG, "New incoming connection");

	// adding this connection fd to the pollfds for further action (messaging / joining channel/ etc.)
	pollfd user_pollfd = {new_fd, POLLIN, 0};
	this->_pollfds.push_back(user_pollfd);

	// client ist ein pair client <pollfd, User*> keine Map
	// ip rausholen () --> als integer kommt der und der muss umgewandelt werden in (1. byte )
	// int umwandeln zum string ip addresse (1. byte rechts ist die linke Zahl bei IP addresse)
	// create a new User with 

	// create a new User object with listening fd and set the listening port
	User* new_user = new User(new_fd, ntohs(s_address.sin_port));
	// User* new_user = new User(fd, ntohs(s_address.sin_port));	//bitshifting necessary? I think ntohs macht das schon
	if (new_user->getState() == false)
	{
		log.printStringCol(REGULAR, "DELETE USER FUNCTION HERE."); 	
		//deleteUser(new_user);	//check with test server what happens
		return ;
	}

	//add new content to server::map users<fd, User*>
	this->_users.insert(std::make_pair(new_fd, new_user));

	std::cout << "New User " << new_user->getUsername() << " connected on port: " << new_fd << std::endl; //new_user.getPort();
	std::cout << "New User " << inet_ntoa(s_address.sin_addr) << ":" << ntohs(s_address.sin_port) << " (" << new_fd << ")" << std::endl;
}

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

/* Erases file descriptor from vector _users and _pollfds.
   Maybe also delete user from channel? how to check internal? */
void Server::disconnectUser(User* user)
{
	if (user)
		log.printStringCol(LOG, "DISCONNECTING USER");


	// // maybe in try / catch in case of fd cannot be find/invalid fd?
	// // User* user = this->_users.at(fd);
	// int user_fd = getUserfd(user);
	// if (user_fd < 0)
	// {
	// 	log.printStringCol(RED, "Error: no user found.");
	// 	serverError(4);
	// }

	// user->leave();		// leave channels
	// // erase this user from std::map by its fd
	// this->_users.erase(user_fd);

	// // erase user from pollfds
	// for (_pfds_iterator = _pollfds.begin(); _pfds_iterator != _pollfds.end(); _pfds_iterator++)
	// {
	// 	if (_pfds_iterator->fd == user_fd)
	// 	{
	// 		this->_pollfds.erase(_pfds_iterator);
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

/* Function prints User credentials: nickname, username, fullname fd */
// void Server::printUser()
// {
// 	for (std::map<int, User*>::iterator iter = users.begin(); iter != users.end(); iter++)
// 	{
// 		std::cout << "nickname: " << iter.getNickname() << " | username: " << iter->getUsername() << " | fullname: " << iter->getFullName();
// 		std::cout << " | port: " << iter.getFD() << std::endl;
// 	}

// }


void Server::sendPing()
{
	time_t current_time = std::time(0);
	int timeout = this->_timeout;

	for (std::map<int, User*>::iterator iter = _users.begin(); iter != _users.end(); iter++)
	{
		if (current_time - ((*iter).second->getLastPing()) >= static_cast<int>(timeout))
			serverError(5);		//disconnect user / delete user / etc.
		else if ((*iter).second->getState() == true)		//online
			(*iter).second->write("PING " + (*iter).second->getNickname());
	}
}


// close() eigentlich verboten! wieso?





/* 	It extracts the first connection request on the queue of pending connections for the
listening socket, sockfd, creates a new connected socket, and returns a new file descriptor referring to that socket.  The
newly created socket is not in the listening state.  The original socket sockfd is unaffected by this call.

The argument sockfd is a socket that has been created with socket(2), bound to a local address with bind(2), and is
listening for connections after a listen(2).

The argument addr is a pointer to a sockaddr structure. This structure is filled in with the address of the peer socket, as
known to the communications layer. The exact format of the address returned addr is determined by the sockets address
family (see socket(2) and the respective protocol man pages). When addr is NULL, nothing is filled in; in this case, addrlen is
not used, and should also be NULL. */



// if (_pfds_iterator->revents != POLLIN)		//e.g. POLLHUP (in explanations.txt) when POLLHUP then disconnect and break
// {
// 	setServerStatus(false);
// 	log.printStringCol(RED, "Error: unexpected result. Nothing to read. Connection will be disabled.");
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

























// NOT SURE ABOUT LOOP
// OLD VERSION:
// if (this->_pfds_iterator->revents == POLLIN)
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
			// 	log.printStringCol(LOG, "A new user is being connected to server");
			// 	connectNewUser();
			// 	log.printStringCol(LOG, "--- Done. User connected to server");
			// 	break ;
			// }	

			// if ((this->_pfds_iterator->revents & POLLIN) == POLLIN)
			// {
			// 	// // when POLLIN on other pollfd.fd than [0] it means a command (PRIVMSG, etc..) has been send.
			// 	// for (std::vector<pollfd>::iterator iter_poll = _pollfds.begin(); iter_poll != _pollfds.end(); iter_poll++)
			// 	// {
			// 	// 	if (iter_poll->revents == POLLIN)
			// 	this->_users[iter_poll->fd]->receiveData(this);		// receive(this)	// implement receive Funktion in User
			// 	// }
			// 	// break ;
			// 	log.printStringCol(LOG, "User receive Data loop --- end");
			// }
			// else
			// {
			// 	// wenn kein POLLIN dann error Nachricht und rausbrechen
			// 	std::cout << " (this->_pfds_iterator->revents != POLLIN)" << std::endl;
			// 	std::cout << "error msg and disconnect Server" << std::endl;
			// 	this->setServerStatus(false);
			// 	break ;
			// }