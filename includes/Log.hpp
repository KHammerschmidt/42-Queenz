#ifndef LOG_HPP
# define LOG_HPP

#include <iostream>
#include <string>

#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

/* LOG MESSAGES */
#define WELCOME_BEFORE 			"LOG: WELCOME TO OUR SERVER @42-Queenz.fr.42..."
#define SERV_LISTENING			"LOG: SERVER IS LISTENING..."
#define NEW_CONNECTION			"LOG: NEW INCOMING CONNECTION."

/* ERROR MESSAGES */
#define INVALID_PARAMS 			"ERROR: INVALID PARAMETERS.\nUSAGE: ./IRCSERV <VALID PORT> <PASSWORD>"
#define ERR_SOCKET_CONNECTION 	"ERROR: SOCKET CONNECTION TO SERVER FAILED."
#define ERR_POLL 				"ERROR: POLL() FAILED."
#define ERR_ACCEPT 				"ERROR: ACCEPT() FAILED."
#define ERR_SEND 				"ERROR: SEND() FAILED."
#define ERR_USER_CREDENTIALS 	"ERROR: NO USER FOUND BY THESE CREDENTIALS."
#define ERR_ANY_OTHER_ERR 		"ERROR: ANOTHER UNSPECIFIED ERROR OCCURRED."
#define ERR_USER_REGISTRY		"ERROR: USER COULD NOT BE CREATED."

/* WARNING MESSAGES */
#define ERR_PING_TIMEOUT 		"WARNING: PING TIMED OUT."
#define ERR_POLLIN				"WARNING: SERVER IS SHUTTING DOWN. NO POLLIN DETECTED."

/* COMMAND MESSAGES */
#define ERR_UNKNOWNCOMMAND(source, command)	"421 " + source + " " + command + " :Unknown command"


enum MSG_STATE { REGULAR, LOG, CRITICAL, WARNING };

struct Log
{
	static void printStringCol(int state, std::string msg);
	static void printStringNoNewline(std::string msg);

	template <typename T>
	static void print_vector(std::vector<T> vctr);
};

#endif