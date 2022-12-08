#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <map>
#include <string>

class User;
class Server;

// enum {channel_creator, channel_operator, channel_member};


class Channel
{

private:
	std::string _name;
	std::string	_topic;
	std::vector<Channel *> _channels;
	std::vector<User *> _users;


	std::string _mode;
	std::map<int, std::string> user_mode;
	std::vector<User *> invited;


	// std::string mode;

	// std::map<std::string, User*> current_members; 	//list of members in channel
	// std::map<std::string, User*> channel_operators; // channel operators can perform
	// std::map<std::string, User*> channel_creator; 	//saves the one creator of the channel (first person who enters it), creator is also automatically an operator

public:
	Channel();
	~Channel();

	void 		setName(const std::string name);
	std::string getName() const;

	void 		setTopic(const std::string topic);
	std::string getTopic() const;
	void 		changeTopic(const std::string newTopic);

	void		addUser(User *user);
	void		deleteUser(User *user);

	void static createChannel();
	void		deleteChannel();

	void setMode(const std::string mode);
	std::string getMode() const;
	//void changeMode(const std::string mode);
	//lists all available modes and returns the valid one or an error
	//void selectNewMode();





};



	// INVITE  - Invite a client to an invite-only channel (mode +i)
	void invite();
	// KICK    - Eject a client from the channel
	void kick();
	// MODE    - Change the channel's mode, as well as members' privileges
	void mode();
	// PRIVMSG - Sending messages to the channel (mode +n, +m, +v)
	void privmsg();
	// TOPIC   - Change the channel topic in a mode +t channel
	void topic();


	void giveOpPrivileges(const User& name);
	void giveCreatPrivileges(const User& name);
	//channel creator becomes channel operator (except for +channels)


	// mode function: take nickname as param and affect privileges given to user;
	// maybe implement 1 or 2? need to execute 5 functions for channel operators
	void executeMode(); 	// function should call respective functions to execute them entioned modes
	//     O - give "channel creator" status;
	//     o - give/take channel operator privilege;
	//     k - set/remove the channel key (password);
	//     l - set/remove the user limit to channel;
	// I deleted all the modes that are not relevant for us (e.g. channel flags and modes)
 */



	// BITTE UPDATE OB ICH ETWAS VON DEN CHANNELS VERGESSEN HABE :)


#endif
