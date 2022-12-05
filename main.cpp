#include "../includes/Server.hpp"
#include <csignal>
#include <iostream>

bool stop = false;

void handler(int) { stop = true; }

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

	server.run();

	return 0;
}