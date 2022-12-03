#include "Server.hpp"

Server::Server(char** argv)
{
	//set current time and other needed vars
	Log::printWelcomeToServer();

	setPort(argv[1]);
	setPassword(argv[2]);
	setStatus(true);
	setHostname();

	this->_socket = newSocket();

	this->_timeout = 1 * 60 * 1000; // == 1 minute

	memset(&_pollfds, 0, sizeof(this->_pollfds));
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
void Server::setStatus(bool status) { this->_status = status; }
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
	// Create a stream socket to receive incoming connections on (in IPv4 domain/Internet domain with default protocol)
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		std::cout << "ERROR WHEN CREATING SOCKER" << std::endl; 				// throw Server::socket_error("ERROR: SOCKET FAILED \n");
		exit(-1);
	}

	// Forcefully attaching socket to the port
	// allow socket fd to be reusable
	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)))	//SO_REUSEADDR | SO_REUSEPORT,
	{
		close(sockfd);
		std::cout << "ERROR WHEN SETTING OPTIONS FOR SOCKET" << std::endl; 				// throw Server::socket_error("ERROR: SET SOCKET OPTIONS FAILED\n");
		exit(-1);
	}

	// set socket to NON_BLOCKING mode allowing it to return any data that the system has in it's read buffer, but won't wait for it
	// the incoming connections will also be nonblocking because they inherit that state from listening socket
	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(sockfd);
		std::cout << "ERROR WHEN SETTING SOCKET TO NON_BLOCKING MODE" << std::endl;				// throw Server::socket_error("ERROR: SET SOCKET OPTIONS FAILED\n");
		exit(-1);
	}

	// bind the socket
	struct sockaddr_in serv_address;

	// Clear address structure, should prevent some segmentation fault and artifacts
	bzero((char *) &serv_address, sizeof(serv_address));

	// convert unsigned short int to big-endian network byte as expected from TCP protocol standards
	serv_address.sin_family = AF_INET;
	serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_address.sin_port = htons(this->_port);

	//specify the networks from which socket is accepting connection requests --> bind socket to current IP address on selected port
	// assigns the address specified by addr to the socker referred to by sockfd
	if (bind(sockfd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0)
	{
		close(sockfd);
		std::cout << "ERROR: BIND SOCKET FAILED" << std::endl; //throw Server::socket_error("some error text");
		exit(-1);
	}

	// enable socket to listen for requests || set liste back log
	if ((listen(sockfd, serv_address.sin_port) < 0))
		std::cout << "ERROR: socket unable to listen to requests" << std::endl;		// throw Server::socket_error("some error text");

	return sockfd;
}


//now that connection has been established between stream sockets any data transfer call can be performed
// read()/write() within poll loop
//out of band data exchange through send()/recv(

void Server::run()
{
	//initialise pollfd struct in Constructor
	//pollfd struct is included in header poll.h
	this->_pollfds.push_back(pollfd());			//add to vector<pollfds> an element to the end called pollfd (woher kommen die variablen?)
	this->_pollfds.back().fd = this->_socket;	// set up initial listening socket
	this->_pollfds.back().events = POLLIN;		// block until new data to read (even: new data to read)

	Log::printString("Server is listening...");

	//get User as a vector or User objects
	std::vector<User*> user_list = getUsers();


	while (this->_status)
	{

		if (poll(_pollfds.begin().base(), _pollfds.size(), -1) < 0)
			std::cout << "Error: while polling from sockfd" << std::endl;	//throw Server::runtime_error(std)("Error while polling from fd.");

		std::vector<pollfd>::iterator iter;
		for (iter = this->_pollfds.begin(); iter != this->_pollfds.end(); iter++)
		{
			if (iter->events == 0)		//this means to revents to perform
				continue;

			if ((iter->events & POLLHUP) == POLLHUP)
			{
				//disconnect
				break;
			}

			if ((iter->revents & POLLIN) == POLLIN)
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