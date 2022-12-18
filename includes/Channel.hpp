#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <map>
#include <vector>
#include <string>

class User;
class Server;

 //enum channels_operators {channel_creator = 1, channel_operator = 2, channel_member = 3};


class Channel
{

private:
	std::string _name;
	std::string	_topic;
 	//static int i;
	//static int j;
	//static int k;



	/*User status*/
	//int	_user_status; //c->creator, o->operator, u->normal user
	
	
	// std::string _mode;
	// std::map<int, std::string> user_mode;
	// std::vector<User *> invited;


	std::string mode;

	// std::map<int, User*> _channel_members; 	//list of members in channel-> key[i]
	// std::map<int, User*> _channel_operators; // channel operators can perform-> key[j]
	// std::map<int, User*> _channel_creator; 	//saves the one creator of the channel (first person who enters it), creator is also automatically an operator -> key[k]

public:
	Channel();
	Channel(const std::string name);
	~Channel();

	void 		setName(const std::string name);
	void		setUserStatus(int status);
	std::string getName() const;
	int			getUserStatus();

	std::map<int, User*>  getChannelMembers();
	std::map<int, User*>  getChannelOperators();
	std::map<int, User*>  getChannelCreator();

	// void 		setTopic(const std::string topic);
	// std::string getTopic() const;
	// void 		changeTopic(const std::string newTopic);

	void		addUser(User *user);//, Server *server);
	// void		deleteUser(User *user);

	void		deleteChannel();
	//void setMode(const std::string mode, const User &user1, const User &user2);
	//void print_vector(std::vector<std::string> vctr);



	//void setMode(const std::string mode, User *user_op,  User *user_not_op);
	//std::string getMode() const;
	//void changeMode(const std::string mode);
	//lists all available modes and returns the valid one or an error
	//void selectNewMode();


	// // INVITE  - Invite a client to an invite-only channel (mode +i)
	// void invite();
	// // KICK    - Eject a client from the channel
	// void kick();
	// // MODE    - Change the channel's mode, as well as members' privileges
	// void mode();
	// // PRIVMSG - Sending messages to the channel (mode +n, +m, +v)
	// void privmsg();
	// // TOPIC   - Change the channel topic in a mode +t channel
	// void topic();


	void giveOpPrivileges(User *user_not_op, std::string nickname_user_op);
	bool returnPrivilegesStatus(std::string user_nickname);

	//void giveCreatPrivileges(const User& name);
	// //channel creator becomes channel operator (except for +channels)


	// // mode function: take nickname as param and affect privileges given to user;
	// // maybe implement 1 or 2? need to execute 5 functions for channel operators
	// void executeMode(); 	// function should call respective functions to execute them entioned modes
	//     O - give "channel creator" status;
	//     o - give/take channel operator privilege;
	//     k - set/remove the channel key (password);
	//     l - set/remove the user limit to channel;
	// I deleted all the modes that are not relevant for us (e.g. channel flags and modes)



	// BITTE UPDATE OB ICH ETWAS VON DEN CHANNELS VERGESSEN HABE :)
};

#endif
