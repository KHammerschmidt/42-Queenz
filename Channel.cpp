#include "Channel.hpp"

Channel::Channel() {}
Channel::~Channel() {}

void Channel::setName(const std::string& name)
{
	//checks if valid:
	// must begin with &#!+
	// lenght <= 50
	// no spaces, no conrol G, no comma, no colon in name
	//duplication of channels: channel names must not be reused

	this->name = name;
}

std::string& Channel::getName()
{
	return this->name;
}

void setTopic(const std::string& topic);		//implement
std::string& getTopic();						//implement

void ChangeTopic(const std::string& newTopic)
{
	setTopic(newTopic);
}
