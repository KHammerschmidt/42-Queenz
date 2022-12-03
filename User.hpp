#ifndef USER_HPP
#define USER_HPP

// namespace irc
// {

class User
{

private:
	std::string _username;
	std::string _nickname;
	std::string	_fullname;

	// std::string	_nick_user_host;	//need hostname for that
	// std::string _hostname;	--> gethostbyname(char* name);	//42-Queenz.42.fr
	// std::string _hostaddr;

	std::vector<polldfs>::iterator	pollfds_iterator;

	int	_fd;				//every user has their own fd
	std::string buffer;		//command Zwischenspeicher
	int	_port;				//port
	ClientState _state;		//what are the states?

	enum {channel_creator, channel_operator, channel_member};

	std::map<Channel&, rights>			channelRights;		//list of rights connected to user and channel
	std::map<std::string, Channel*>		channel_lst;		//list of channels that user is member of

	std::string _channel_name: 		//name + @ (don't know which name)


public:
	User(const std::string& nick, const std::string& user, const std::string& fullname);
	~User();

	std::string getNickname() const;
	std::string getUsername() const;
	std::string getFullname() const;

	void setNickname();
	void setUsername();
	void setFullname();

	std::string setNickUserHost()

	// std::string getHostname() const;
	void setHostname();
};

User::User(const std::string& nick, const std::string& user, const std::string& fullname)
	: _nickname(nick), _username(user), _fullname(fullname)
{

}

std::string setNickUserHost()
{
	std::stringstream ss;

	ss << this->_nickname + "!" + this->_username + "@" + Server::hostname;

	return ss.str;
}

// void User::setHostname()
// {
// 	this->_hostname = Server::getHostname();
// 	this->_hostname = gethostbyname();
// }

// std::string User::getHostname()
// {
// 	return Server::getHostname();



// }	// end namespace irc



#endif