
#include <poll.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include "Server.hpp"

int main(int argc, char** argv)
{
	if (argc != 3)
		std::cout << "Error: Invalid paramters\n ./ircserv <port> <password>" << std::endl;

	Server server(argc[1], argc[2]);

	Server::init_server(argc, argv);



	// while (true)
	// {
	// 	int poll_fd = poll();
	// 	if (poll_fd == 0)
	// 		//timeout (set timeout to sth in milliseconds
	// 	else if (poll_fd == -1)
	// 		//process failed
	// 	else if (poll_fd == 1)
	// 		//only one descriptor is ready to be processed (only processed if it is the listening socket)
	// 	else
	// 		//poll_fd > 1: multiple descriptors are waiting to be processed, allowd simultaneous connection with descriptors in queue of listening socket
	// }