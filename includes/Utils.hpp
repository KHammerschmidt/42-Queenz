#ifndef UTILS_HPP
# define UTILS_HPP

#include <iostream>
#include <string>
#include <vector>

struct Utils
{
	static std::vector<std::string> split(std::string str, std::string delimiter);
	static int check_characters(std::string str);
};



#endif