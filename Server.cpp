#include "Server.hpp"


int Server::getSocket() const { return this->_socket; }
int Server::getPort() const { return this->_port; }
int Server::getSocket() const { return this->_socket; }
int Server::getTimeout() const { return this->_timeout; }
bool Server::getStatus() const { return this->_status; }
std::string Server::getPassword() const { return this->_password; }
std::string Server::getHostname() const { return this->_hostname; }

Server::~Server() { std::cout << "Disconnecting... bye bye" << std::endl; }
// delete channels / delete users

Server::Server(char** argv)
{
	Log::printWelcomeToServer();

	if (checkPort(argv[1])
		this->_port = argv[1];
	this->_password = argv[2];

	this->_hostname = "42-Queenz.42.fr";

	this->_socket = newSocket();
	this->_timeout = 3 * 60 * 1000;	// Standard: 3 Minuten

	this->_pollfds.push_back(pollfd());
	memset(&_pollfds, 0, sizeof(this->_pollfds));
	this->_pollfds.back().fd = this->_socket;
	this->_pollfds.back().revents = POLLIN;

	// this->_lastPing = ;
	// this->_online;
	// this->_max_online;
}


void Server::setServerStatus(bool status) { this->_status = status; }

/* Checks for valid port and sets server status to running if input is valid. */
void Server::checkPort(std::string port_str)
{
	int port = std::atoi(port_str.c_str());

	if (isdigit(port))
	{
		std::cout << "Error: Invalid port paramter" << std::endl;
		//throw Server::port_error("Invalid port paramter\n");
		setServerStatus(false);
	}

	setServerStatus(true);
}

int Server::newSocket(void)
{

	// socket klasse: server socket und client socket die dann erben
	//
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		std::cout << "ERROR WHEN CREATING SOCKER" << std::endl;
		// throw Server::socket_error("ERROR: SOCKET FAILED \n");
		setServerStatus(false);  		//exit(-1);
	}
	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)))
	{
		close(sockfd);
		std::cout << "ERROR WHEN SETTING OPTIONS FOR SOCKET" << std::endl;
		// throw Server::socket_error("ERROR: SET SOCKET OPTIONS FAILED\n");
		setServerStatus(false);  		//exit(-1);
	}

	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(sockfd);
		std::cout << "ERROR WHEN SETTING SOCKET TO NON_BLOCKING MODE" << std::endl;
		// throw Server::socket_error("ERROR: SET SOCKET OPTIONS FAILED\n");
		setServerStatus(false);  		//exit(-1);
	}

	struct sockaddr_in serv_address;
	bzero((char *) &serv_address, sizeof(serv_address));
	serv_address.sin_family = AF_INET;
	serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_address.sin_port = htons(this->_port);

	if (bind(sockfd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0)
	{
		close(sockfd);
		std::cout << "ERROR: BIND SOCKET FAILED" << std::endl;
		//throw Server::socket_error("some error text");
		exit(-1);
	}

	if ((listen(sockfd, serv_address.sin_port) < 0))
	{
		std::cout << "ERROR: socket unable to listen to requests" << std::endl;
		// throw Server::socket_error("some error text");
		exit(-1);
	}

	return sockfd;
}

void Server::run()
{
	Log::printString("Server is listening...");
	// std::vector<User*> user_list = getUsers();		//get User as a vector or User objects		//necessary?

	// timeout: nach 3 Minuten schließt sich wieder der server wenn kein request kommt
	// with -1 it is not blocking // standard von IBM ist 3 Minuten
	if (poll(_pollfds.begin().base(), _pollfds.size(), this->_timeout) < 0)
	{
		std::cout << "Error: while polling from sockfd" << std::endl;
		//throw Server::runtime_error(std)("Error while polling from fd.");
		close(this->_socket);
		break ;
	}

	//send ping		//necessary?

	// loop through vector pollfds that list all file descriptors.
	for (pollds_iterator = this->_pollfds.begin(); pollds_iterator != this->_pollfds.end(); pollds_iterator++)
	{
		if (pollds_iterator->events == 0)				// this means the file descriptor is not yet ready to be read
		{
			// perform ping to see if already timed out?
			// sendPing(); //lastPing();
			continue;
		}


		if (_pollfds[0].revents == POLLIN)		// warum sollte erste position vom vector anzeigen, dass es einen neuen User gibt
			// add a new user
		else
		{
			// loop durch vector und guck of revents == POLLIN sind --> receive a message


		}
		if (pfds[0].revents == POLLIN)
			acceptUser();
		else
			for (std::vector<pollfd>::iterator it = pfds.begin(); it != pfds.end(); ++it)
				if ((*it).revents == POLLIN)
					this->users[(*it).fd]->receive(this);



			if (pollds_iterator->revents != POLLIN)		//e.g. POLLHUP (in explanations.txt) when POLLHUP then disconnect and break
			{
				setServerStatus(false);
				Log::printString("Error: unexpected result. Nothing to read. Connection will be disabled.");
				break ;
			}

			// this means the file descriptor is ready to be readable
			if (pollds_iterator->fd == this->_socket)
			{
				Log::printString("Listening socket is readable");
				Log::printString("Accept all incoming connections that are queued on the listening socket before calling poll again");

				// accept the incoming connections. accept returns a new file descriptor  if new_fd < 0: shows failure, we will end the server
				int fd = accept(this->_socket, NULL, NULL);


				struct sockaddr_in s_address;
				socklen_t s_size = sizeof(s_address);
				int fd = accept(this->_socket,(sockaddr *) &s_address, &s_size);

				It extracts the first
       connection request on the queue of pending connections for the
       listening socket, sockfd, creates a new connected socket, and
       returns a new file descriptor referring to that socket.  The
       newly created socket is not in the listening state.  The original
       socket sockfd is unaffected by this call.

	          The argument sockfd is a socket that has been created with
       socket(2), bound to a local address with bind(2), and is
       listening for connections after a listen(2).

	          The argument addr is a pointer to a sockaddr structure.  This
       structure is filled in with the address of the peer socket, as
       known to the communications layer.  The exact format of the
       address returned addr is determined by the socket's address
       family (see socket(2) and the respective protocol man pages).
       When addr is NULL, nothing is filled in; in this case, addrlen is
       not used, and should also be NULL.

				if (fd < 0)
				{
					// differentiation to EWOULDBLOCK????
					// If the socket is marked nonblocking and no pending connections are present on the queue, accept() fails with the error EAGAIN or EWOULDBLOCK.
					Log::printString("Error: accept() failed");
					setServerStatus(false);
					break ;
				}


				// accept returns a new fd that displays the connection
				// add incoming connection to pollfds struct
				struct sockaddr_in s_address;
				socklen_t s_size = sizeof(s_address);

				Log::printString("New incoming connection");

				// users[fd] = new User(fd, address);
				// if (Bedingung erfüllt)		//valid username / valid password??
				// 	// user[fd]->register();



				User* new_user = new User(fd, this->_hostname, ntohs(s_address.sin_port));
				users.insert(std::make_pair(fd, new_user));
				std::cout << "New User " << new_user->getUsername() << " connected on port: " << fd << std::endl; //new_user.getPort();

				this->_pollfds.push_back(pollfd());
				this->_pollfds.back().fd = fd;
				this->_pollfds.back().events = POLLIN;

				std::cout << "New User " << inet_ntoa(address.sin_addr) << ":" << ntohs(address.sin_port) << " (" << fd << ")" << std::endl;

				Client *client = new Client(fd, hostname, ntohs(s_address.sin_port));
				_clients.insert(std::make_pair(fd, client));




			}





			if ((iter->revents & POLLIN) == POLLIN)	// the file descriptor is ready and we can read/receive from it
			{
				if (iter->fd == this->_socket)
				{
					//connect;
					break;
				}
			}
			// clientMessage(iter->fd);
		}
	}
}





	// std::string hostname = gethostbyname();
	// std::cout << hostname << std::endl;
//open points: sethostname, gethostname in Client




/* Function returns a vector with User objects, extracted from member type std::map<int, User*>. */
std::vector<User*> Server::getUsers()
{
	std::vector<User *>users = std::vector<User*>();
	for (std::map<int, User *>::iterator iter = this->users.begin(); iter != this->users.end(); iter++)
		users.push_back(iter->second);

	return users;
}

/* Function prints User credentials: nickname, username, fullname fd */
void Server::printUser()
{
	for (std::map<int, User*>::iterator iter = users.begin(); iter != users.end(); iter++)
	{
		std::cout << "nickname: " << iter.getNickname() << " | username: " << iter->getUsername() << " | fullname: " << iter->getFullName();
		std::cout << " | port: " << iter.getFD() << std::endl;
	}

}

// addUser
// deleteUser





void Server::error(int err_code)
{
	if (err_code == 0)
		return ;
	else if (err_code == 1)
		// error bliblablub
}















	// init_poll();
	//initialise pollfd struct in Constructor
	// this->_pollfds.push_back(pollfd());				//add to vector<pollfds> the struct pollfd from <sys/poll.h>
	// memset(&_pollfds, 0, sizeof(this->_pollfds));
	// this->_pollfds.back().fd = this->_socket;		// we want to listen to our socket (other example: 0 for STDIN)
	// this->_pollfds.back().events = POLLIN;			// the events I'm interested in (POLLIN) if a fd is ready to listen to then we read/recv from it