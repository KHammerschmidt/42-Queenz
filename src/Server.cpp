#include "../includes/Server.hpp"

/* ======================================================================================== */
/* ----------------------------- CONSTRUCTOR/DESTRUCTOR ----------------------------------  */
Server::Server(char** argv)
	: _serv_address(), _users(), _channels()		//, _channels_by_name()
{
	setPort(argv[1]);
	this->_password = argv[2];

	setNewSocket();
	this->_timeout = 3 * 60 * 1000;

	pollfd server_fd = { this->_sockfd, POLLIN, 0};
	memset(&_pollfds, 0, sizeof(this->_pollfds));
	this->_pollfds.push_back(server_fd);
}


Server::~Server()
{
	for (user_iterator = _users.begin(); user_iterator != _users.end(); user_iterator++)
		std::cout << (*user_iterator).first << "  " << (*user_iterator).second->getNickname() << std::endl;

	for (std::map<int, User*>::iterator iter = this->_users.begin(); iter != this->_users.end(); iter++)
	{
		Log::printStringCol(WARNING, "DISCONNECTING AND DELETING USER IN DESTRUCTOR");
		disconnectUser((*iter).second->getFd(), false);
		delete ((*iter).second);
	}

	this->_users.clear();



	// for (std::map<std::string, User*>::iterator iter = this->_channel_users.begin(); iter != this->_channel_users.end(); iter++)
	// 	deleteUser((*iter).second);

	// for (std::vector<Channel*>::iterator iter = this->_channels.begin(); iter != this->_channels.end(); iter++)
	// 	deleteChannel(*iter);


	// for (std::map<std::string, Channel*>::iterator iter = this->_channels.begin(); iter != _channels.end(); iter++)
	// 	deleteChannel((*iter).second);

	this->_channel_users.clear();
	this->_channels.clear();
	this->_channel_users.clear();
	delete this;


	// this->_channels_by_name.clear();

}

/* ======================================================================================== */
/* ------------------------------------ MAIN LOOP ----------------------------------------  */
/* Makes the server listen to incoming requests and loops over these requests to identify a POLLIN event.
   If file descriptor is readable, the pollfds struct indicates if either a new user registered or another
   command has been sent. The function then calls the respective function. */
void Server::run()
{
	while (this->getServerStatus() == true)
	{
		Log::printStringCol(REGULAR, SERV_LISTENING);

		if (poll(_pollfds.begin().base(), _pollfds.size(), this->_timeout) < 0)
			serverError(2);
		for (pfds_iterator = this->_pollfds.begin(); pfds_iterator < this->_pollfds.end(); pfds_iterator++)
		{
			if (pfds_iterator->revents == 0)
				continue;

			//POLLHUP will signal that the connection was closed from both sides (server & client)
			// if ((pfds_iterator->revents & POLLHUP) == POLLHUP)
			// {
			// 	std::cout << "DISCONNECT USER AFTER POLLHUP" << std::endl;
			// 	disconnectUser(pfds_iterator->fd, false);
			// 	break ;
			// }

			if ((pfds_iterator->revents & POLLIN) == POLLIN)
			{
				if (this->_pollfds[0].revents == POLLIN)
				{
					connectNewUser();
					break ;
				}

				this->_users[pfds_iterator->fd]->onUser();

				if (this->_users[pfds_iterator->fd]->getState() == DELETE)
					disconnectUser(pfds_iterator->fd, false);
			}
		}
	}
}

/* ======================================================================================== */
/* --------------------------------- USER CONNECTION --------------------------------------  */
/* To register, user sends request via server socket, the only outer connection. A new fd for user is created with accept(). */
void Server::connectNewUser()
{
	int new_fd;
	struct sockaddr_in s_address;
	socklen_t s_size = sizeof(s_address);

	new_fd = accept(this->_sockfd,(sockaddr *) &s_address, &(s_size));
	if (new_fd < 0)
		return ;
	else
		Log::printStringCol(LOG, NEW_CONNECTION);

	pollfd user_pollfd = {new_fd, POLLIN, -1};
	this->_pollfds.push_back(user_pollfd);

	User* new_user = new User(new_fd, s_address, this);

	this->_users.insert(std::make_pair(new_fd, new_user));

	std::cout << "New User on port: new_fd: " << new_fd << " | inet_ntoa: " << inet_ntoa(s_address.sin_addr);
	std::cout << ":" << ntohs(s_address.sin_port) << " (" << new_fd << ")" << std::endl;
}

/* Makes user leave all channels and erases the user and its file descriptor from server. */
void Server::disconnectUser(int fd, bool state)
{
	Log::printStringCol(LOG, "LOG: DISCONNECTING USER");

	try
	{
		// 1 get user to be disconnected
		User* tmp_user = this->_users.at(fd);
		std::vector<Channel*> channel_to_delete;

		for (channel_iterator = _channels.begin(); channel_iterator != _channels.end(); channel_iterator++)
		{
			Channel* tmp_channel = *channel_iterator;
			std::cout << "CURRENT CHANNEL NAME: " << (*channel_iterator)->getName() << std::endl;

			std::cout << "---CHANNEL OPERATOR SIZE: " << tmp_channel->_channel_operators.size() << std::endl;
			if (tmp_channel->_channel_operators.size() > 0)
			{
				for (std::vector<User*>::iterator it_a = tmp_channel->_channel_operators.begin(); it_a != tmp_channel->_channel_operators.end(); it_a++)
				{
					if ((*it_a)->getNickname() == tmp_user->getNickname())
					{
						tmp_channel->_channel_operators.erase(it_a);
						break ;
					}
				}
			}

			std::cout << "---CHANNEL MEMBER SIZE: " << tmp_channel->_channel_members.size() << std::endl;
			if (tmp_channel->_channel_members.size() > 0)
			{
				for (std::vector<User*>::iterator it_a = tmp_channel->_channel_members.begin(); it_a != tmp_channel->_channel_members.end(); it_a++)
				{
					if ((*it_a)->getNickname() == tmp_user->getNickname())
					{
						tmp_channel->_channel_members.erase(it_a);
						break ;
					}
				}
			}


			if (tmp_channel->_channel_members.size() == 0)
				channel_to_delete.push_back(tmp_channel);

		}

		for (size_t i = 0; i < channel_to_delete.size(); i++)
		{
			for (channel_iterator = this->_channels.begin(); channel_iterator != _channels.end(); channel_iterator++)
			{
				if ((*channel_iterator)->getName() == channel_to_delete[i]->getName())
				{
					std::cout << "DELETING CHANNEL BECAUSE ITS EMPTY" << std::endl;
					this->_channels.erase(channel_iterator);
					break;
				}
			}
		}

		//delete out of multimap
		// std::cout << "CHANNEL USERS DELETE" << std::endl;
		// for(std::multimap<std::string, User*>::iterator it = _channel_users.begin(); it != _channel_users.end(); it++)
		// {
		// 	if (((*it).second->getNickname()).compare(tmp_user->getNickname()) == 0)
		// 	{
		// 		_channel_users.erase(it);
		// 	}
		// }


		std::vector<pollfd>::iterator iter;
		for (iter = _pollfds.begin(); iter != _pollfds.end(); iter++)
		{
			if (iter->fd == fd)
			{
				this->_pollfds.erase(iter);
				close(fd);
				break ;
			}
		}

		//delete allocated user memory and delete from this->_users map
		if (state == false)
		{
			delete this->_users.at(fd);
			this->_users.erase(fd);
		}

	}
	catch(const std::out_of_range& e) 	{
	}
}


/* ======================================================================================== */
/* -------------------------------------- GETTERS ----------------------------------------  */
int 		Server::getPort() const { return this->_port; }
int 		Server::getTimeout() const { return this->_timeout; }
int 		Server::getServerFd(void) { return (this->_sockfd); }
bool 		Server::getServerStatus() const { return this->_serverRunningStatus; }
std::string Server::getPassword() const { return this->_password; }
sockaddr_in	Server::getAddr() const { return this->_serv_address; }

User* Server::getUser(int fd)
{
	for (std::map<int, User*>::iterator iter = this->_users.begin(); iter != this->_users.end(); iter++)
	{
		if ((*iter).first == fd)
			return ((*iter).second);
	}
	return NULL;
}

/* Function returns a vector with User objects, extracted from member type std::map<int, User*>. */
std::vector<User*> Server::getUsers() const
{
	std::vector<User *> users_temp = std::vector<User*>();

	for (std::map<int, User *>::const_iterator iter = _users.begin(); iter != _users.end(); iter++)
		users_temp.push_back(iter->second);

	return users_temp;
}

/* ======================================================================================== */
/* -------------------------------------- SETTERS ----------------------------------------  */
void Server::setServerStatus(bool new_status) { this->_serverRunningStatus = new_status; }
void Server::setPort(std::string port_str)
{
	this->_port = std::atoi(port_str.c_str());
	setServerStatus(true);
}

/* Creates a new socket, sets options and binds it to the server and listens on socket. */
void Server::setNewSocket(void)
{
	this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_sockfd < 0)
		serverError(1);

	int enable = 1;
	if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) != 0)
		serverError(1);

	if (fcntl(this->_sockfd, F_SETFL, O_NONBLOCK) < 0)
		serverError(1);

	memset(&(this->_serv_address), 0, sizeof(this->_serv_address));
	this->_serv_address.sin_family = AF_INET;
	this->_serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
	this->_serv_address.sin_port = htons(this->_port);

	if (bind(this->_sockfd, (struct sockaddr *) &(this->_serv_address), sizeof(this->_serv_address)) < 0)
		serverError(1);

	if ((listen(this->_sockfd, this->_serv_address.sin_port) < 0))
		serverError(1);
}

/* ======================================================================================== */
/* -------------------------------- HELPER FUNCTIONS -------------------------------------  */
/* Prints error messages, closes server fd and sets server status to false which shuts down the server. */
void Server::serverError(int code)
{
	switch(code)
	{
		case 1:
			Log::printStringCol(CRITICAL, ERR_SOCKET_CONNECTION);
			break;
		case 3:
			Log::printStringCol(CRITICAL, ERR_ACCEPT);
			break;
		case 4:
			Log::printStringCol(CRITICAL, ERR_USER_CREDENTIALS);
			break;
		case 5:
			Log::printStringCol(CRITICAL, ERR_PING_TIMEOUT);
			break;
		case 6:
			Log::printStringCol(CRITICAL, ERR_SEND);
			break;
		default:
			Log::printStringCol(CRITICAL, "SERVER IS SHUTTING DOWN...");
			break;
	}

	close(this->_sockfd);
	setServerStatus(false);
}


/* ======================================================================================== */
/* -------------------------- DELETION OF USERS / CHANNEL ---------------------------------  */
//delete and disconnect
void Server::deleteUser(int fd)
{
	std::cout << "HERE DELETE USER " << std::endl;
	delete getUser(fd);
}

//delete in channel class as well & user
void Server::deleteChannel(Channel* channel)
{
	std::cout << "HERE DELETE CHANNEL" << std::endl;
	delete channel;
}
