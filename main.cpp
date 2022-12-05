#include "includes/Server.hpp"
#include <csignal>
#include <iostream>

bool running = true;		//link running with command receive to set it right 

void handler(int) { running = false; }

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Error: Invalid paramters. Usage: ./ircserv <port> <password>" << std::endl;
		return -1;
	}

	Server server(argv);


	// kennst du dich mit signals aus? :D
	signal(SIGINT, handler);

	while (running)
		server.run();

	return 0;
}