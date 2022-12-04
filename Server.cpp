#include "Server.hpp"

int Server::getSocket() const { return this->_sockfd; }
int Server::getPort() const { return this->_port; }
int Server::getTimeout() const { return this->_timeout; }
bool Server::getStatus() const { return this->_serverRunningStatus; }
std::string Server::getPassword() const { return this->_password; }
std::string Server::getHostname() const { return this->_hostname; }

Server::~Server() { std::cout << "Disconnecting... bye bye" << std::endl; }	// delete channels / delete users // more to add???

Server::Server(char** argv)
{
	log.printWelcomeToServer();

	setPort(argv[1]);
	this->_password = argv[2];
	this->_hostname = "42-Queenz.42.fr";
	
	newSocket();
	this->_timeout = 3 * 60 * 1000;	// Standard timeout: nach 3 Minuten schließt sich wieder der server wenn kein request kommt
	this->_error = -1;

	this->_pollfds.push_back(pollfd());
	memset(&_pollfds, 0, sizeof(this->_pollfds));
	this->_pollfds.back().fd = this->_sockfd;
	this->_pollfds.back().revents = POLLIN;

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

void Server::serverError(int code)
{
	// implement enum { NO_ERROR = -1, INPUT_PARAM = 0; ERROR_SOCKET_CONNECTION = 2, ERROR_POLL = 3, ERROR_ACCEPT = 4, ANOTHER_ERROR = 5}
	if (code == -1)		
		return ;
	else if (code == 0)
		log.printString("Error: Invalid input paramter");
	else if (code == 1)
		log.printString("Error: The socket connection of server 42-Queenz.fr could not be established.");
	else if (code == 2)
		log.printString("Error: while polling from sockfd");
	else if (code == 3)
		log.printString("Error: accept() failed");
	else
		log.printString("Another error.");

	close(this->_sockfd);
	setServerStatus(false);
	exit(-1);
}

void Server::run()
{
	while (this->getServerStatus() == true)
	{
		log.printString("Server is listening...");

		// timeout: -1 infinite timeout // standard von IBM ist 3 Minuten
		if (poll(_pollfds.begin().base(), _pollfds.size(), this->_timeout) < 0)
			serverError(2);

		// loop through vector pollfds that lists all file descriptors on open connections
		for (_pfds_iterator = this->_pollfds.begin(); _pfds_iterator != this->_pollfds.end(); _pfds_iterator++)
		{
			// this means the file descriptor is not yet ready to be read, 
			if (_pfds_iterator->events == 0)
				continue;

			// _pollfds[0] stellt das erste pollfd struct dar in dem die server socket gespeichert ist. Nur über diese socket können sich neue User registrieren.
			if (this->_pollfds[0].revents == POLLIN)
				registerNewUser();
			else
			{
				// when POLLIN on other pollfd.fd than [0] it means a command (PRIVMSG, etc..) has been send.
				for (std::vector<pollfd>::iterator iter_poll = _pollfds.begin(); iter_poll != _pollfds.end(); iter_poll++)
				{
					if (iter_poll->revents == POLLIN)
						this->_users[iter_poll->fd]->receive();		// receive(this)	// implement receive Funktion in User
				}
			}
		}
	}
}

void Server::registerNewUser()
{
	// check for max user size

	struct sockaddr_in s_address;
	socklen_t s_size = sizeof(s_address);

	// accept the incoming connetion; accept() returns a new fd that displays the connection that has to be added to pollfds vector
	int fd = accept(this->_sockfd,(sockaddr *) &s_address, &s_size);
	if (fd < 0)
	{
		// differentiation to EWOULDBLOCK???? // if the socket is marked nonblocking and no pending connections are present on the queue, accept() fails with the error EAGAIN or EWOULDBLOCK.
		log.printString("Error: accept() failed");
		serverError(2);
	}

	log.printString("New incoming connection");

	User* new_user = new User(fd, ntohs(s_address.sin_port));
	if (new_user->getState() == false)
	{
		delete new_user; // or delete in Destructor
		return ;
	}

	//add new content to server::map(fd, User*)
	_users.insert(std::make_pair(fd, new_user));

	std::cout << "New User " << new_user->getUsername() << " connected on port: " << fd << std::endl; //new_user.getPort();
	std::cout << "New User " << inet_ntoa(s_address.sin_addr) << ":" << ntohs(s_address.sin_port) << " (" << fd << ")" << std::endl;

	// adding this connection fd to the pollfds for further action (messaging / joining channel/ etc.)
	this->_pollfds.push_back(pollfd());
	this->_pollfds.back().fd = fd;
	this->_pollfds.back().events = POLLIN;

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














	// init_poll();
	//initialise pollfd struct in Constructor
	// this->_pollfds.push_back(pollfd());				//add to vector<pollfds> the struct pollfd from <sys/poll.h>
	// memset(&_pollfds, 0, sizeof(this->_pollfds));
	// this->_pollfds.back().fd = this->_sockfd;		// we want to listen to our socket (other example: 0 for STDIN)
	// this->_pollfds.back().events = POLLIN;			// the events I'm interested in (POLLIN) if a fd is ready to listen to then we read/recv from it






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
// 	log.printString("Error: unexpected result. Nothing to read. Connection will be disabled.");
// 	break ;
// }