#include "../includes/Channel.hpp"

// to create a channel -> in USER: if (user parse tipes #join) -> Channel:createChannel (with static method); if doesnt work, delete createChannel and in User do new Channel()

Channel::Channel() {};
Channel::Channel(std::string channel_name) {setName(channel_name);}
Channel::~Channel() {}

void        Channel::setName(const std::string name){
    std::string name_init = "&#!+";
    unsigned long i = 0;

    if (name.empty())
        return ;
    while (name_init[i])
    {
        if (name_init[i] != name[0])
            break ;
        i++;
        if (i == 4)
            return ;
    }

    //kann ueberspringen
    // if (name.size() > 50)
    //     this->_name = name.resize(50);
    // else
    //     this->_name = name;

    for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        if ((*it)->_name == this->_name)   
        { 
            std::cout << "Channel already exists!" << std::endl;
             delete this;
             return ;
        }
    }
    i = 0;
    while (i < name.length())
    {
        if ((this->_name[i] == ' ') || (this->_name[i] == ':') || (this->_name[i] == ','))
        {
            std::cout << "Invalid Channel name" << std::endl;
            return ;
        }
    }
    _channels.push_back(this);
    /*implement function error handling instead of all this. Maybe use some boolean, return true or false.*/
}
// std::string Channel::getName() const{return this->_name;}

void Channel::createChannel(const std::string channel_name)
{
        new Channel(channel_name);
}

void        Channel::deleteChannel()
{
    if (_users.size() == 0)
        delete this; //suicide
}


// void        Channel::setTopic(const std::string topic){this->_topic = topic;}
// std::string Channel::getTopic() const{return this->_topic;}

// void Channel::changeTopic(const std::string newTopic) {
//     setTopic(newTopic); 
//     std::cout << "The topic of the channel was updatet!" << std::endl;
// }


// void		Channel::addUser(User *newUser){
//     _user.push_back(newUser);
// }
// void		Channel::deleteUser(User *user){
//      for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); ++it)
//         if (*it == user)
//             _users.erase(it);
    
//     deleteChannel();

// }





