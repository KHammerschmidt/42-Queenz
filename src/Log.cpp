#include "../includes/Log.hpp"

void Log::printStringCol(int state, std::string msg)
{ 
	switch(state)
	{
		case REGULAR:
			std::cout << BLUE << msg << std::endl; 
			break;
		case LOG: 
				std::cout << GREEN << msg << std::endl;
				break;
		case WARNING:
				std::cout << YELLOW << BOLD << msg << std::endl;
				break ;
		case CRITICAL:
				std::cout << RED << BOLD << msg << std::endl;
				break;
		default:
			std::cout << msg << std::endl;
			break;
	}
}

void Log::printStringNoNewline(std::string msg)
{
	std::cout << msg;
}
