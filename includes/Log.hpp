#ifndef LOG_HPP
# define LOG_HPP

#include <iostream>
#include <string>

#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

class Log
{

	// log class: log.print("CRITCIAL/SOMETHIGN", mesg);
	// colour printing??? how???
public:


	void printString(std::string color, std::string msg);
	void printWelcomeToServer();
	// void printString(std::string msg);
	void printStringNoNewline(std::string msg);
};

#endif