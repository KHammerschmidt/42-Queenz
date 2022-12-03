#include "Server.hpp"

irc::Server::Server(const std::string& port, const std::string& pw)
	: password(pw)
{
	this->port = set_port(port);

	if (this->port < 0)
		std::cout << "ERROR: Invalid port" std::endl; 	//throw Server::port_error("Invalid port\n");
}
irc::Server::~Server() {}

int irc::Server::set_port(const std::string& port_str)
{
	int port = std::atoi(port_str.c_str());

	if (isdigit(port) == 0)
		return -1;

	return port;
}

void irc::Server::set_password(const std::string& pw)
{
	this->password = pw;
}

int	irc::Server::init_server(const std::string& port, const std::string& password)
{
	set_port(port);
	set_up_socket();
}

int Server::set_up_socket(void)
{
	// Create a stream socket to receive incoming connections on
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		std::cout << "ERROR" << std::endl; 		// throw Server::socket_error("ERROR: SOCKET FAILED \n");

	// Forcefully attaching socket to the port
	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(enable)));
		std::cout << "ERROR" << std::endl; 		// throw Server::socket_error("ERROR: SET SOCKET OPTIONS FAILED\n");

	/* subject: set socket to NON_BLOCKING mode allowing it to return any data that the system has in it's read buffer
	// for that socket, but it won't wait for the data.	*/
	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1)
		std::cout << "ERROR" << std::endl;		// throw Server::socket_error("ERROR: SET SOCKET OPTIONS FAILED\n");


	struct sockaddr_in serv_address;

	// Clear address structure, should prevent some segmentation fault and artifacts
	bzero((char *) &serv_address, sizeof(serv_address));

	// convert unsigned short int to big-endian network byte as expected from TCP protocol standards
	serv_address.sin_family = AF_INET;
	serv_address.sin_addr.s_addr = INADDR_ANY;				//getaddr??
	serv_address.sin_port = htons(this->port);				//oder hier erst parsen --> atoi(config.get("port").c_str()));

	//bind socket to current IP address on selected port
	if (bind(sockfd, (struct sockaddr *)& serv_address, sizeof(serv_address)) < 0)
		std::cout << "ERROR: BIND SOCKET FAILED" << std::endl; //throw Server::socket_error("some error text");


	// enable socket to listen for requests


	if (listen(fd, address.sin_port) < 0)
		error("listen", true);


	// Let socket be able to listen for requests
	if (listen(sockfd, MAX_CONNECTIONS) < 0)
		throw std::runtime_error("Error while listening on socket.");
	return sockfd;


	return 0;

}