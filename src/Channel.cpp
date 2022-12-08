#include "../includes/Channel.hpp"

// to create a channel -> in USER: if (user parse tipes #join) -> Channel:createChannel (with static method); if doesnt work, delete createChannel and in User do new Channel()

//add flags to user (as var in class user): bool creator = false and bool op = false


Channel::Channel() {setName(const std::string name);_maxUsers = 200;}
Channel::~Channel() {}

//NAME
void        Channel::setName(const std::string name){
    std::string name_init = "&#!+";
    int i = 0;

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

    if (name.size() > 50)
        this->_name = name.resize(50);
    else
        this->_name = name;

    i = 0;
    while (i < name.size())
    {
        if ((this->_name[i] == ' ') || (this->_name[i] == ':') || (this->_name[i] == ','))
        {
            std::cout << "Invalid Channel name" << std::endl;
            return ;
        }
    }

    for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channel.end(); ++it)
    {
        if (it->_name == this->_name)   
        { 
            std::cout << "Channel already exists!" << std::endl;
            return ;
        }
    }
    _channels.push_back(*this);
    /*implement function error handling instead of all this. Maybe use some boolean, return true or false.*/
}
std::string Channel::getName() const{return this->_name;}

void static Channel::createChannel()
{
        new Channel();
}

void        Channel::deleteChannel()
{
    if (_users.size() == 0)
        delete this; //suicide
}


void        Channel::setTopic(const std::string topic){this->_topic = topic;}
std::string Channel::getTopic() const{return this->_topic;}

void Channel::changeTopic(const std::string newTopic) {
    setTopic(newTopic); 
    std::cout << "The topic of the channel was updatet!" << std::endl;
}


void		Channel::addUser(User *newUser){
    if (_users.size() > _maxUsers)
    {
        std::cout << "Channel already reached the maximal number of users!"
        return;
    }
    _user.push_back(newUser);
    if (_users.size() == 0)
        newUser->creator = true;
}

void		Channel::deleteUser(User *user){
     for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); ++it)
        if (*it == user)
            _users.erase(it);
    
    deleteChannel();

}

void        Channel::setMode(const std::string mode, const User &user1, const User &user2)
{
    if (user1->op == false || user1->creator == false) 
        return ;

    // O - give "channel creator" status;
    if (mode == 'O')
    {
        if (user1->creator == true)
            user2->creator ==true;
    }
    // Ban (+b)
    if (mode == 'b')
        deleteUser(user2);
    // INVITE  - Invite a client to an invite-only channel (mode +i)
    if (mode == 'i')
        addUser(user2);
    //     o - give/take channel operator privilege;
    if (mode == 'o')
    {
        if (user2->op == true)
            user2->op == false;
        else
            user2->flag == true;
    }
    //     l - set/remove the user limit to channel;
    if (mode == 'l')
        _maxUsers = 10^1000000;


}



