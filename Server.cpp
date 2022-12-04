#include "Server.hpp"

Server::Server(char** argv)
{
	//set current time and other needed vars
	Log::printWelcomeToServer();

	setPort(argv[1]);
	setPassword(argv[2]);
	setServerStatus(true);
	setHostname();

	this->_socket = newSocket();

	this->_timeout = 3 * 60 * 1000;	//Standard Wert 3 Minuten
}

Server::~Server()
{
	std::cout << "Disconnecting... bye bye" << std::endl;
}

int Server::getSocket() const { return this->_socket; }
int Server::getPort() const { return this->_port; }
int Server::getSocket() const { return this->_socket; }
int Server::getTimeout() const { return this->_timeout; }
bool Server::getStatus() const { return this->_status; }
std::string Server::getPassword() const { return this->_password; }
std::string Server::getHostname() const { return this->_hostname; }

void Server::setPassword(const std::string& pw) { this->_password = pw; }
void Server::setHostname() { this->_hostname = "42-Queenz.42.fr"; }
void Server::setServerStatus(bool status) { this->_status = status; }
void Server::setPort(const std::string& port_str)
{
	int port = std::atoi(port_str.c_str());

	if (isdigit(port))
	{
		std::cout << "Error: Invalid port paramter" << std::endl;
		exit(-1); 			//throw Server::port_error("Invalid port paramter\n");
	}

	this->_port = port;
}

int Server::newSocket(void)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		std::cout << "ERROR WHEN CREATING SOCKER" << std::endl;
		// throw Server::socket_error("ERROR: SOCKET FAILED \n");
		exit(-1);
	}
	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)))
	{
		close(sockfd);
		std::cout << "ERROR WHEN SETTING OPTIONS FOR SOCKET" << std::endl;
		// throw Server::socket_error("ERROR: SET SOCKET OPTIONS FAILED\n");
		exit(-1);
	}

	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(sockfd);
		std::cout << "ERROR WHEN SETTING SOCKET TO NON_BLOCKING MODE" << std::endl;
		// throw Server::socket_error("ERROR: SET SOCKET OPTIONS FAILED\n");
		exit(-1);
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
	// init_poll();
	//initialise pollfd struct in Constructor
	this->_pollfds.push_back(pollfd());				//add to vector<pollfds> the struct pollfd from <sys/poll.h>
	memset(&_pollfds, 0, sizeof(this->_pollfds));
	this->_pollfds.back().fd = this->_socket;		// we want to listen to our socket (other example: 0 for STDIN)
	this->_pollfds.back().events = POLLIN;			// the events I'm interested in (POLLIN) if a fd is ready to listen to then we read/recv from it

	Log::printString("Server is listening...");
	// std::vector<User*> user_list = getUsers();		//get User as a vector or User objects		//necessary?

	while (this->_status)
	{
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

		for (iter_poll = this->_pollfds.begin(); iter_poll != this->_pollfds.end(); iter_poll++)
		{
			if (iter_poll->events == 0)				// this means the file descriptor is not yet ready to be read
				continue;

			if (iter_poll->revents != POLLIN)		//e.g. POLLHUP (in explanations.txt) when POLLHUP then disconnect and break
			{
				setServerStatus(false);
				Log::printString("Error: unexpected result. Nothing to read. Connection will be disabled.");
				break ;
			}


			if (iter_poll->fd == this->_socket)
			{
				Log::printString("Listening socket is readable");
				Log::printString("Accept all incoming connections that are queued on the listening socket before calling poll again");

				// accept the incoming connections. if
				// if new_fd < 0: shows failure, we will end the server
				int fd = accept(this->_socket, NULL, NULL);

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