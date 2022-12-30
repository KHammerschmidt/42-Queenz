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
#define ERR_ERRONEUSUSERNAME 	"Erroneus username\r\n"
#define ERR_NICKNAMEINUSE 		"Nickname is already in use\r\n"

#define ERR_PASSWDMISMATCH 		"Password incorrect\r\n"
#define ERR_ALREADYREGISTERED 	"You may not reregister\r\n"


class Command
{

	private:
			User* 		_user;
			Server* 	_server;

			std::string query;
			std::string user_receiver; 				//by msg and notice needed to check if user receiver exist
			std::string _command_message;
			std::string _reply_message;
			bool		command_state;
			bool		reply_state;
			std::string user_command;

			std::string text;
			std::string prefix;

			std::vector<std::string> _args;			//[/COMMAND, NICKNAME, RECEIVER_NICKNAME, MESSAGE];

			std::vector<User*> channels_replies;

			std::string sender_nickname;
			int 		receiver_fd;

public:
		Command(User* user, Server* server, std::string message);
		~Command();

		std::map<int, std::string>	send_map;

		int getReceiverFd() const;
		bool getCommandState() const;
		bool getReplyState() const;
		std::string getCommandMessage() { return this->_command_message; }
		std::string getReply() { return this->_reply_message; }

		User 	*return_user_in_server(const std::string nickname_receiver);
		//Channel	*return_channel_in_server(const std::string channel_name, Server *_server);

		bool new_channel_to_create(const std::string channel_name);
		bool joinInputFormatCheck(std::string command, std::string channel_name, std::string prefix_channel);
		bool handleDoubleUserError(std::string channel_name);
		bool valid_channel(std::string channel_name);

		std::string return_string_all_users_in_channel(const std::string channel_name);//, Server *server, User *user);
		bool find_user_in_channel(std::string channel_name, std::string nickname);
		Channel *return_channel(std::string channel_name);
		User *return_user_in_multimap(std::string channel_name, std::string nickname);
		std::string return_string_all_users_OP_in_channel(const std::string channel_name);


		void sendPrivMsgUser(User* user, std::string msg);

		void sendJoin(User* user, std::string message);
		void sendChannelMsg(std::string text, std::string channel_name);
		void sendQuit(User* user);
		void sendPart(std::string msg);
		void sendMode(std::string msg);
		void setMode(std::string mode, std::string channel_name, std::string nickname);
		void sendTopic(std::string msg);
		void sendKick(std::string msg);
		void sendInvite(std::string msg);
		void sendFile(std::string msg);



		void register_pass();
		void register_cap();
		void register_nickname();
		void register_username();
		void send_pong();

		bool parse_command(std::string message);
		bool msg_quit(std::string message);
		bool check_free_nickname(const std::string& nickname);
		void err_command(std::string err_num, std::string cmd, std::string code);

		void getWelcomeReply(User* user);
		void send_replies_to_channel();
		std::string put_reply(User* user, std::string err_num, std::string code);
		std::string put_reply_cmd(User* user, std::string err_num, std::string cmd, std::string code);

};

#endif
