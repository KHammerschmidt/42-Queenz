#include "../includes/Log.hpp"

void Log::printString(std::string color, std::string msg) { std::cout << color << BOLD << msg << std::endl; }

enum MSG_CONDITION { DEFAULT, LOG, CRITICAL, WARNING };

void Log::printWelcomeToServer()
{
	printString(RED, "Welcome to our server 42-Queenz...");
}

// void Log::printString(std::string msg)
// {
// 	std::cout << msg << std::endl;
// 	std::cout << "/033[34m%-4i msg /33{35m" << std::endl; //break;
// }

void Log::printStringNoNewline(std::string msg)
{
	std::cout << msg;
}

// void Log::printMsg()

// void Log::printErrorMsg(int MSG_CONDITION, const std::string& msg)
// {
// 	switch(MSG_CONDITION)
// 	{

// 	"\033[34m%-4i \033[33m%-9s \033[35m"

// 		case LOG: std::cout << "/033[34m%-4i msg /33{35m" << std::endl; break;
// 		case CRITICAL: std::cout << msg << std::endl; break;
// 		case WARNING: std::cout << msg << std::endl; break;
// 		default: std::cout << msg << std::endl; break ;
// 	}
// }