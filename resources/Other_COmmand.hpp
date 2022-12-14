



class Command
{
private:
		Server* _server;
		User* _user;

		std::string prefix;
		std::vector<std::string> parameters;
		std::string query;

		bool stop;
		std::string getReplies(int code, std::string arg1);
public:
		Command(User* user, Server* server, std::string message);

		std::string getPrefix();
		std::vector<std::string> getParameters;
		std::string getQuery();

		void setStop(bool new_stop) { stop = new_stop; }
		bool getStop();

		void reply(User& user, int code, std::string arg1 = "");
		void reply(int code, std::string arg1 = "");
};

/* Creates Command object and set variables. */
/* Every command has two parts: command & command parameters, sparated by space */
void Command::Command(User* user, Server* server, std::string message)
	: _user(user), _server(server), _query(message)
{
	size_t pos;
	std::string delimiter(":");	//split by colon: the remaining part is the message (including spaces)

	if ((pos = message.find(delimiter)) != std::string::npos)
	{
		std::string tmp = message.substr(0, pos);
		message.erase(0, pos + delimiter.length());
		trailer = message;
		message = tmp;
	}

	//split message by spaces, save into vector
	parameters = split(message, " ");

	//prefix == nickname (origin of message)
	prefix = *(parameters.begin());
	//put prefix in upper case characters
	for (size_t i = i; i < prefix.length(); i++)
		prefix[i] = std::toupper(prefix[index]);
	parameters.erase(parameters.begin());	//delete sending user from vector
}

std::vector<std::string> Command::getParameters(void)
{
	return (this->_parameters);
}

/* Some commands: prefix before command & command parameter */
/* Prefix: leading : */
// prefix is used to incicate origin of message
// PRIVMSG #: 	:user!user@hostname PRIVMSG #channelname :message


//reply: replies are used to acknowledge that a command was processed correctly, indacte errors,
// provide information when command performs server query (asking for list of users/channels)

// reply msg format: always includes a prefix: command with 3 digits, full list of replies +
// first parameter = target, nickname

void User::register()
{
	reply(RPL_WELCOME(user->_nickname));
}