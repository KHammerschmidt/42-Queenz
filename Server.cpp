#include "Server.hpp"

Server::Server(const std::string& port, const std::string& pw)
	: password(pw)
{
	this->port = set_port(port);

	if (this->port < 0)
		std::cout << "ERROR PARSING: Invalid port" << std::endl; 	//throw Server::port_error("Invalid port\n");
}

Server::~Server() {}

int Server::set_port(const std::string& port_str)
{
	int port = std::atoi(port_str.c_str());

	if (isdigit(port))
		return -1;

	return port;
}

void Server::set_password(const std::string& pw)
{
	this->password = pw;
}

void	Server::init_server()
{
	set_up_socket();
}

void Server::set_up_socket(void)
{
	// Create a stream socket to receive incoming connections on
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		std::cout << "ERROR WHEN CREATING SOCKER" << std::endl; 				// throw Server::socket_error("ERROR: SOCKET FAILED \n");

	// Forcefully attaching socket to the port
	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)))
		std::cout << "ERROR WHEN SETTING OPTIONS FOR SOCKET" << std::endl; 				// throw Server::socket_error("ERROR: SET SOCKET OPTIONS FAILED\n");

	// subject: set socket to NON_BLOCKING mode allowing it to return any data that the system has in it's read buffer, but won't wait for it
	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)
		std::cout << "ERROR WHEN SETTING SOCKET TO NON_BLOCKING MODE" << std::endl;				// throw Server::socket_error("ERROR: SET SOCKET OPTIONS FAILED\n");


	// ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
	// !!!! I DONT'T GET THIS SOCKADDR_IN PART YET! WILL WORK ON IT TOMORROW :( !!!!!
	struct sockaddr_in serv_address;

	// Clear address structure, should prevent some segmentation fault and artifacts
	bzero((char *) &serv_address, sizeof(serv_address));

	// convert unsigned short int to big-endian network byte as expected from TCP protocol standards
	serv_address.sin_family = AF_INET;
	serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_address.sin_port = htons(this->port);				//oder hier erst parsen --> atoi(config.get("port").c_str()));

	//bind socket to current IP address on selected port || binds the socket to all available interfaces
	if (bind(sockfd, (struct sockaddr *) &serv_address, sizeof(serv_address)) < 0)
		std::cout << "ERROR: BIND SOCKET FAILED" << std::endl; //throw Server::socket_error("some error text");

	// enable socket to listen for requests
	if ((listen(sockfd, serv_address.sin_port) < 0))
		std::cout << "ERROR: socket unable to listen to requests" << std::endl;		// throw Server::socket_error("some error text");

	pollfds.push_back(pollfd());
	pollfds.back().fd = sockfd;
	pollfds.back().events = POLLIN;

}