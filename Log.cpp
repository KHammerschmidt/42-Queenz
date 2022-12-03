#include "Log.hpp"

void Log::printWelcomeToServer()
{
	printString("Welcome to our server 42-Queenz...");
}


void Log::printString(const std::string& msg)
{
	std::cout << msg << std::endl;
}