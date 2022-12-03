#ifndef CHANNEL_HPP
# define CHANNE_HPP

#include <map>
#include <string>

#include "User.hpp"
#include "Server.hpp"

class User;
class Server;

enum {channel_creator, channel_operator, channel_member};

// namespace irc
// {

class Channel
{

private:
	std::string name;
	std::string	topic;

	// std::string mode;

	// std::map<std::string, User*> current_members; 	//list of members in channel
	// std::map<std::string, User*> channel_operators; // channel operators can perform
	// std::map<std::string, User*> channel_creator; 	//saves the one creator of the channel (first person who enters it), creator is also automatically an operator

	// std::map<std::string, Channel*>	channel_list;	// list of all current channels

public:
	Channel();
	~Channel();

	void setName(const std::string& name);
	std::string& getName();

	void setTopic(const std::string& topic);
	std::string& getTopic();
	void ChangeTopic(const std::string& newTopic);

	void setMode(const std::string& mode);
	std::string& getMode();
	void changeMode(const std::string& mode);

	//lists all available modes and returns the valid one or an error
	void selectNewMode();

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


	// channels are created when the first user joins it
	void createChannel(const std::string& name, const User& creator);
	// channels are deleted when the last user leaves it
	void deleteChannel();

	// function to reference channel?
	// any client can reference the channel using the name of the channel

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




	// BITTE UPDATE OB ICH ETWAS VON DEN CHANNELS VERGESSEN HABE :)
};


// }	//end namespace irc

#endif