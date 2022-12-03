// just for open points to be implemented


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