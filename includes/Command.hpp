#ifndef COMMAND_HPP
# define COMMAND_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Log.hpp"
#include "Utils.hpp"


#define ERR_UNKNOWNCOMMAND 		"Unknown command\r\n"
#define ERR_NEEDMOREPARAMS		"Not enough parameters\r\n"

#define ERR_ERRONEUSNICKNAME 	"Erroneus nickname\r\n"
#define ERR_NICKNAMEINUSE 		"Nickname is already in use\r\n"

#define ERR_PASSWDMISMATCH 		"Password incorrect\r\n"
#define ERR_ALREADYREGISTERED 	"You may not reregister\r\n"


class Command
{

private:
		User* _user;
		Server* _server;

		std::string query;
		bool 		authenticated;
		bool		command_state;
		bool		reply_state;
		std::string user_command;



		std::string user_receiver; 				//by msg and notice needed to check if user receiver exist
		std::string text;
		std::string prefix;
		std::vector<std::string> _args;	//[/COMMAND, NICKNAME, RECEIVER_NICKNAME, MESSAGE];


		//channel related
		std::vector<User*> channels_replies;
		bool _state;							//ready to send data or not

		bool stop;
		std::string getReplies(int code, std::string arg1);

public:
		std::string sender_nickname;
		bool	_valid_command;
		int 	receiver_fd;

		std::string _command_message;
		std::string _reply_message;

		std::map<int, std::string>	send_map;

		bool getCommandState() { return this->command_state; }
		bool getReplyState() { return this->reply_state; }

		std::string getCommandMessage() { return this->_command_message; }
		std::string getReply() { return this->_reply_message; }

		User 	*return_user_in_server(const std::string nickname_receiver);
	//	Channel	*return_channel_in_server(const std::string channel_name, Server *_server);

		bool new_channel_to_create(const std::string channel_name);
		bool joinInputFormatCheck(std::string command, std::string channel_name, std::string prefix_channel);
		bool handleDoubleUserError(std::string channel_name);
		bool valid_channel(std::string channel_name);


		std::string return_string_all_users_in_channel(const std::string channel_name);//, Server *server, User *user);
		bool find_user_in_channel(std::string channel_name, std::string nickname);
		Channel *return_channel(std::string channel_name);





		Command(User* user, Server* server, std::string message);
		// std::vector<std::string> split(std::string str, std::string delimiter);

		std::string getPrefix() const;
		std::vector<std::string> getParameters();
		std::string getQuery();


		bool getCommandState() const;
		void execute();


		void setNickname(const std::string& nickname);

		void sendPrivMsgUser(User* user, std::string msg);
		void nick(User* user, const std::string& msg);
		void sendJoin(User* user, std::string message);
		void sendChannelMsg(std::string text, std::string channel_name);
		void sendQuit(User* user);
		void sendPart(std::string msg);
		void sendMode(std::string msg);
		void setMode(std::string mode, std::string channel_name, std::string nickname);





		// ------------ KATHI
		bool check_free_nickname(const std::string& nickname);
		void register_nickname(void);
		void err_command(std::string err_num, std::string cmd, std::string code);
		bool parse_command(std::string message);
		bool getReplyState(void) const;
		void send_pong();
		std::string getUserCommand() const;

		void register_username(void);
		void getWelcomeReply(User* user);
		void register_pass(void);
		void register_cap(void);

		void print_vector(std::vector<std::string> vctr);
		std::string put_reply(User* user, std::string err_num, std::string code);
		std::string put_reply_cmd(User* user, std::string err_num, std::string cmd, std::string code);
};

#endif
