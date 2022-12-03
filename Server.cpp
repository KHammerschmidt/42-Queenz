#include "Server.hpp"

Server::Server(char** argv)
{
	//set current time and other vars
	std::cout << "Welcome to our server 42-Queenz..." << std::endl;

	this->_port = set_port(argv[1]);
	this->_password = set_password(argv[2]);
	this->_socket = new_socket();
}

Server::~Server()
{
	std::cout << "Disconnecting... bye bye" << std::endl;
}

int Server::set_port(const std::string& port_str)
{
	int port = std::atoi(port_str.c_str());

	if (isdigit(port))
	{
		std::cout << "Error: Invalid port paramter" << std::endl;
		exit(-1); 			//throw Server::port_error("Invalid port paramter\n");
	}

	return port;
}

const std::string& Server::set_password(const std::string& pw) { return pw; }

int Server::new_socket(void)
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

	// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
	// bind the socket
	// !!!! I DONT'T GET THIS SOCKADDR_IN PART YET! WILL WORK ON IT TOMORROW :( !!!!!
	// in server class?
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

	pollfds.push_back(pollfd());
	pollfds.back().fd = sockfd;
	pollfds.back().events = POLLIN;

	//now that connection has been established between stream sockets any data transfer call can be performed
	// read()/write() within poll loop
	//out of band data exchange through send()/recv()

	return sockfd;
}

void Server::execute()
{
	//here poll loop
}