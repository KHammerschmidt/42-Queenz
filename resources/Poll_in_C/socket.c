#include "socket.h"

int main (int argc, char **argv)
{
	int len, rc, on = 1;
	int listen_sd = -1, new_sd = -1;
	int desc_ready, compress_array = FALSE;
	int	end_server = FALSE;
	int close_conn;
	char buffer[80];
	struct sockaddr_in6 addr; //ipv6 - Linux IPv6 protocol-> To bind an AF_INET6 socket to any process, the local address
	       					  //should be copied from the in6addr_any variable which has in6_addr type. 
	/*
	   Address format
	           struct sockaddr_in6 {
	               sa_family_t     sin6_family;   // AF_INET6 
	               in_port_t       sin6_port;     // port number 
	               uint32_t        sin6_flowinfo; // IPv6 flow information 
	               struct in6_addr sin6_addr;     // 128-bit IPv6 address 
	               uint32_t        sin6_scope_id; // Scope ID depending on the scope of the address 
	           };

	           struct in6_addr {
	               unsigned char   s6_addr[16];   // IPv6 address
		*/
	int timeout;
	struct pollfd fds[200];
	/*
	 struct pollfd {				 // array of structures
	               int   fd;         // file descriptor 
	               short events;     // requested events
	               short revents;    // returned events 
	           };
	*/
	int nfds = 1, current_size = 0, i, j;










	//Create an AF_INET6 stream socket to receive incoming connections on

	/*socket(int domain, int type, int protocol);
	->domain selects a protocol family that should be used, we use  PF_INET6
	->type: sock_stream
	-> protocolo: a single protocol selected
	
	Stream socket must be in a CONNECTED state before data is sent or received. 
	A connection to another socket is creat with a connect or connectx call.
	Once connected data may be transferred using read and write calls or variants 
	of send or recv calls (last 2 options allows to tramitted also out of band data).
	When session completed a close will be performed.
	*/
	listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
	if (listen_sd < 0)
	{
		perror("socket() failed");
		exit(-1);
	}

	//Allow socket descriptor to be reusable
	
	/* int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
		Manipulates the options associated with a socket
		
		To manipulate option at the socket level, level is specified as SOL_SOCKET.
		Option_name SO_REUSEADDR enables local adress reuse by the rules used in validating adresses supplied in a bind call.
		Option_value and Option_len are used to access option values for setsockopt().
	*/
	rc = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt() failed");
		close(listen_sd);
		exit(-1);
	}


	//Set socket to nonblocking. All of the sockets for the incoming connections will also be non blocking 
	//since they will inherit that state from the listening socket.

	/*int ioctl(int fd, unsigned long request, ...);
		https://man7.org/linux/man-pages/man2/ioctl.2.html -> NOT ALLOWED
	*/
	
	rc = ioctl(listen_sd, FIONBIO, (char *)&on);
	if (rc < 0)
	{
		perror("iotctl() failed");
		close(listen_sd);
		exit(-1);
	}
	

	//Bind the socket
	//int bind(int sockfd, const struct sockaddr *addr,b socklen_t addrlen);

	/*
	When a socket is created with socket() it exists in a name space (adress family), but has not adress assigned to it.
	Bind() assigns the adress specified by addr to the socket referred to by the file descriptor sockfd. Addrelen is the size in bytes.
	Is necessary to assign a local adress using bind() before a SOCK_STREAM may receive connections.
	(struct sockaddr *)&addr: cast to avoid compiler warings
	*/
	memset(&addr, 0, sizeof(addr));
	addr.sin6_family = AF_INET6;
	memcpy(&addr.sin6_addr, &in6addr_any, sizeof((in6addr_any)));
	addr.sin6_port = htons(SERVER_PORT);//These routines convert 16 and 32 bit quantities between network byte order and host byte order. 
	rc = bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0)
	{
		perror("bind() failed");
		close(listen_sd);
		exit(-1);
	}


	//Initialize the pollfd structure
	memset(fds, 0, sizeof(fds));

	//Set up the initial listening socket
	fds[0].fd = listen_sd;
	fds[0].events = POLLIN; //POLLIN:there is a data to read

	//Initialize the timeout to 3 minutes. If no activity -> program ends; time in ms
	timeout = ( 3 * 60 * 1000);





	//Loop waiting for incoming connects or incoming data on any of the connected sockets
	do
	{
		//1) Call poll() and wait 3 minutes for it to complete
		//   int poll(struct pollfd *fds, nfds_t nfds, int timeout);
		
		/*
			Waitsc for one of a set of file descriptors (fds, which is an array of strutures) to become ready to perform I/O.
			ndfs: number of items in fds.
			fd (of fds): contains a file descriptor for an open file (if fd<0, events and revents=0 and fd ignored)
			events(of fds): input param,bit mask specifing the events the application is interested in for fd.
			revents(of fds): output param, filled by the kernel with the events that actually occured. The bits returning here can include any of those specified in events, or one of the values POLLERR, POLLHUP or POLLNVAL.
		
			If no events requesed (and no error) has occured for any file descriptor, poll() blocks until one event occurs
			for timeout seconds (means fd is ready). Timeout can be also interruptepted by a signal handler.
		*/
		printf("Waiting on poll()...\n");
		rc = poll(fds, nfds, timeout);

		//2) Check to see if the poll call failed.
		if (rc < 0)
		{
			perror("poll() failed");
			break ;
		}

		//3) Check to see if the 3 minute time out expired
		if (rc == 0)
		{
			printf("poll() timed out . End Program.\n");
			break;
		}

		//4) One or more descriptors are readable. Need to be which ones they are.
		current_size = nfds;
		for (i = 0; i < current_size; i++)
		{
			//4.1) Loop throught to find the descriptors that returned POLLIN and determine wheter it is the listening or active connection.
			if (fds[i].revents == 0)
				continue;
			//4.2) 		If revents is not POLLIN, it is an unexpected result, log and end the server.
			if (fds[i].revents != POLLIN)
			{
				printf("Error! revents= %d\n", fds[i].revents);
				end_server = TRUE;
				break ;
			}

		//4.4) 
		if (fds[i].fd == listen_sd)
		{
			//4.4.1 Listening descriptor is readable
			printf("Listening socket is readable\n");
			
			//4.4.2 Accept each incoming connection. If accept fails with EWOULDBLOCK, then we have acceppted all of them.
			//		Any other failure on accept will cause us to end the the server.
			do
			{
				//int accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
				/*
					The accept() function shall extract the first connection on the queue of pending connections, create a new socket 
					with the same socket type protocol and address family as the specified socket, and allocate a new file descriptor for that socket.
					If the actual length of the address is greater than the length of the supplied sockaddr structure, the stored address shall be truncated.
					If the listen queue is empty of connection requests and O_NONBLOCK is not set on the file descriptor for the socket, accept() shall block until a connection is present. If the listen() queue is empty of connection requests and O_NONBLOCK is set on the file descriptor for the socket, accept() shall fail and set errno to [EAGAIN] or [EWOULDBLOCK].
					The accepted socket cannot itself accept more connections. The original socket remains open and can accept more connections.
					Upon successful completion, accept() shall return the non-negative file descriptor of the accepted socket. Otherwise, -1 shall be returned and errno set to indicate the error.
					[EAGAIN] or [EWOULDBLOCK]: O_NONBLOCK is set for the socket file descriptor and no connections are present to be accepted.
				*/
				new_sd = accept(listen_sd, NULL, NULL);
				if (new_sd < 0)
				{
					if (errno != EWOULDBLOCK)
					{
						perror("accept() failed");
						end_server = TRUE;
					}
					break;
				}

				//4.3 Add the new incoming connection to the pollfd structure.
				printf("New incoming connection - %d\n", new_sd);
				fds[nfds].fd = new_sd;
				fds[nfds].events = POLLIN;
				nfds++;

			//4.4 Loop back up and accept another incoming connection
			} while (new_sd != -1);
		}
		
		//4.5) This is not the listening socket, therefore an existing connection must be readable
		else
		{
			printf("Descriptor %d is readable \n", fds[i].fd);
			close_conn = FALSE;
			//4.6) Receive all incoming data on this socket before we loop back and call poll again
			do
			{
				//4.7) Receive data on this connection until the recv fails with EWOULDBLOCK. If any other failure occurs,
				//		we will close the connection

				//ssize_t recv(int sockfd, void *buf, size_t len, int flags);
				/*
					Used to receive messages form a socket. They may be used to receive data on both connectionless and connection-oriented sockets.
					Recv with a zero flags argument is equals to read. Returns lenght of the message (if too long will be truncated), othrwise -1 with EWOULDBLOCK in errno.
				*/
				rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
				if (rc < 0)
				{
					if (errno != EWOULDBLOCK)
					{
						perror("Connection closed\n");
						close_conn = TRUE;
						break;
					}
				}
				// 4.8) Check to see if the connection has been closed from client.
				if (rc == 0)
				{
					printf("Connection closed\n");
					close_conn = TRUE;
					break ;
				}
				//4.9) Data was received
				len = rc;
				printf("%d bytes received\n", len);
				//4.10) Echo the data back to client
				
				//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
				/* 
				Transmit a message to another socket.Like recv can be used jsut in a connected status and with a zero flag argument is like write.
				If message too long for buffer returns -1 and if in not blocking mode error EAGAIN or EWOULDBLOCK, otherwise would block. 
				*/
				rc = send(fds[i].fd, buffer, len, 0);
				if (rc < 0)
				{
					perror("send() failed");
					close_conn = TRUE;
					break;
				}
			} while (TRUE);
			
			//5.0) If the  close_conn flag was turned on, we need to clean up this active connection. 
			//	This clean up process includes removing the descriptor.
			if (close_conn)
			{
				close(fds[i].fd);
				fds[i].fd = -1;
				compress_array = TRUE;
			}


		} /* End of existing connection is readable*/

	}; /* End of loop through pollable desciptors*/

	//6) If the compress_array flag was turned on, we need to squeeze togheter the array and decrement the number 
	//   of  file descriptors. We do not need to move back the events and revents fields because the events will always
	//	be POLLIN in this cas, and revents is output.
		if (compress_array)
		{
			compress_array = FALSE;
			for (i = 0; i < nfds; i++)
			{
				if (fds[i].fd == -1)
				{
					for(j = i; j < nfds; j++)
					{
						fds[j].fd = fds[j+1].fd;
					}
					i--;
					nfds--;
				}
			}
		}
	} while (end_server == FALSE); /* End of serving running */

	//7) Clean up all the sockets that are open
	for (i = 0; i < nfds; i++)
	{
		if (fds[i].fd >= 0)
			close(fds[i].fd);
	}


return (0);
}
	