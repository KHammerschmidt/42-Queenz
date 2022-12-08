#ifndef LOG_HPP
# define LOG_HPP

#include <iostream>
#include <string>

#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

# define INVALID_PARAMS "Error: Invalid parameters.\nUsage: ./ircserv <valid port> <password>"

enum MSG_STATE { REGULAR, LOG, CRITICAL, WARNING };

struct Log
{
	
public:
	Log();
	~Log();

	static void printStringCol(int state, std::string msg);
	static void printStringNoNewline(std::string msg);
};

#endif