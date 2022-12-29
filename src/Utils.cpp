#include "../includes/Utils.hpp"

/* Splits a string by a delimiter and saves content in vector. */
std::vector<std::string> Utils::split(std::string str, std::string delimiter)
{
	size_t pos;
	std::string tmp;
	std::vector<std::string> splitted_message;

	while ((pos = str.find(delimiter))!= std::string::npos)
	{
		tmp = str.substr(0, pos);
		splitted_message.push_back(tmp);
		str.erase(0, pos + delimiter.length());
	}

	if (str.length() != 0)
		splitted_message.push_back(str);

	return splitted_message;
}

/* Checks for characters a-z and digits 0-9 + size <= 63 characters. */
int Utils::check_characters(std::string str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (!isalpha(str.c_str()[i]) && !isdigit(str.c_str()[i]))
			return -1;
	}

	if (str.length() >= 63)
		return -1;

	return 1;
}
