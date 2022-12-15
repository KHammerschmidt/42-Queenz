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

#define ERR_NONICKNAMEGIVEN 	"431 :Nickname not given\r\n"
#define ERR_ERRONEUSNICKNAME 	"432: Erroneus nickname\r\n"
#define ERR_NICKNAMEINUSE 		"433 :Nickname is already in use\r\n"

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

		Command(User* user, Server* server, std::string message);
		// std::vector<std::string> split(std::string str, std::string delimiter);

		std::string getPrefix() const;
		std::vector<std::string> getParameters();
		std::string getQuery();


		bool getCommandState() const;
		void execute();

		int find_user_in_server(const std::string nickname_receiver);

		void setNickname(const std::string& nickname);

		void sendPrivMsgUser(User* user, std::string msg);
		//void sendChannelMsg(User* user, std::string msg);
		void nick(User* user, const std::string& msg);
		void sendPrivNoticeUser(User* user, std::string msg);
		void sendJoin(User* user, std::string message);
		//void sendQuit(User* user, const std::string message);


		// ------------ KATHI
		bool set_vars(const std::string& message);
		// int check_characters(std::string str);
		bool check_free_nickname(const std::string& nickname);
		void register_nickname(void);
		void err_command(std::string err_num, std::string cmd, std::string code);
		bool parse_command(std::string message);
		bool getReplyState(void) const;
		void send_pong();
		std::string getUserCommand() const;

		void register_username(void);
		std::string getWelcomeReply(User* user);
		void register_pass(void);
		void register_cap(void);

		void print_vector(std::vector<std::string> vctr);
		std::string put_reply(User* user, std::string err_num, std::string code);
		std::string put_reply_cmd(User* user, std::string err_num, std::string cmd, std::string code);
};


/* Some commands: prefix before command & command parameter */
/* Prefix: leading : */
// prefix is used to incicate origin of message
// PRIVMSG #: 	:user!user@hostname PRIVMSG #channelname :message


//reply: replies are used to acknowledge that a command was processed correctly, indacte errors,
// provide information when command performs server query (asking for list of users/channels)

// reply msg format: always includes a prefix: command with 3 digits, full list of replies +
// first parameter = target, nickname


#endif






// // enum CommandState{ NICK, USER, PING, JOIN, PRIVMSG_CH, PRIVMSG_U };

// // class User;

// class Command
// {
// 	private:
// 		Server* _server;
// 		User* _user;

// 		std::string prefix;
// 		std::vector<std::string> parameters;
// 		std::string query;

// 		bool stop;
// 		std::string getReplies(int code, std::string arg1);

// 	public:
// 		Command(User* user, Server* server, std::string message);

// 		std::string getPrefix();
// 		std::vector<std::string> getParameters;
// 		std::string getQuery();

// 		void setStop(bool new_stop) { stop = new_stop; }
// 		bool getStop();

// 		void reply(User& user, int code, std::string arg1 = "");
// 		void reply(int code, std::string arg1 = "");

// 	public:
// 		Command(Server* server);
// 		~Command();

// 		// void invokeMessage(User* user);

// 		void execute(User* user, const std::string& msg);
// 		void execute_command(User* user, std::vector<std::string> cmds_to_exec);
// 		int extract_command(const std::string& msg);

// 		void print_cmds_to_exec(std::vector<std::string> cmds_to_exec);

// 		// void sendPrivMsgChannel(User *user, const std::string& msg);
// 		// void sendPrivMsgUser(User* user, const std::string& msg);
// 		// void sendPing(User* user, const std::string& msg);

// 		void sendNickname(User* user, std::string message);
// 		void sendUsername(User* user, std::string message);
// 		void sendPong(User* user, std::string message);
// 		void sendJoin(User* user, std::string message);
// 		void sendPrivMsgChannel(User* user, std::string message);
// 		void sendPrivMsgUser(User* user, std::string message);

// };

// #endif
