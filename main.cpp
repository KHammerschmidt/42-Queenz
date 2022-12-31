#include <csignal>
#include <iostream>
#include "includes/Server.hpp"

bool running = false;
void handler(int) { running = true; }

int parse(int argc, char** argv)
{
	if (argc != 3 || std::atoi(argv[1]) <= 0 || std::atoi(argv[1]) > 65535)
		return -1;

	return 0;
}

int main(int argc, char** argv)
{
	if (parse(argc, argv) < 0)
	{
		Log::printStringCol(CRITICAL, INVALID_PARAMS);
		return -1;
	}

	Server* server = new Server(argv);

	signal(SIGINT, handler);

	while (!running)
		server->run();

	system("leaks ircserv");
	std::cout << "EVERYTHING IS CLOSED" << std::endl;

	return 0;
}