#ifndef LOG_HPP
# define LOG_HPP

#include <iostream>
#include <string>

#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

enum MSG_STATE { REGULAR, LOG, CRITICAL, WARNING };

class Log
{
	
public:
	Log();
	~Log();

	void printStringCol(int state, std::string msg);
	void printStringNoNewline(std::string msg);
};

#endif