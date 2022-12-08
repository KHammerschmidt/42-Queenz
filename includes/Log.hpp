#ifndef LOG_HPP
# define LOG_HPP

#include <iostream>
#include <string>

class Log
{

	// log class: log.print("CRITCIAL/SOMETHIGN", mesg);
	// colour printing??? how???
public:

	void printWelcomeToServer();
	void printString(std::string msg);
	void printStringNoNewline(std::string msg);
};

#endif