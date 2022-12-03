#include "Server.hpp"

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Error: Invalid paramters. Usage: ./ircserv <port> <password>" << std::endl;
		return -1;
	}

	// create server object
	Server server(argv);

	// while server status == running, run the server in an endless loop
	while (server.getStatus())
		server.run();

	return 0;
}