#include "../includes/Channel.hpp"

// to create a channel -> in USER: if (user parse tipes #join) -> Channel:createChannel (with static method); if doesnt work, delete createChannel and in User do new Channel()

Channel::Channel() {};
Channel::Channel(std::string channel_name) {setName(channel_name);}
Channel::~Channel() {}

void        Channel::setName(const std::string channel_name){this->_name = channel_name;}
//void		Channel::setUserStatus(int status) {this->_user_status = status;}
std::string Channel::getName() const {return this->_name;}
//int         Channel::getUserStatus() {return this->_user_status;}



void        Channel::deleteChannel()
{
    if (_channel_members.size() == 0)
        delete this; //suicide
}


// void        Channel::setTopic(const std::string topic){this->_topic = topic;}
// std::string Channel::getTopic() const{return this->_topic;}

// void Channel::changeTopic(const std::string newTopic) {
//     setTopic(newTopic); 
//     std::cout << "The topic of the channel was updatet!" << std::endl;
// }


void		Channel::addUser(User *user){//, Server *server){
   static int i = 0, j = 0;//, k = 0;

    if (_channel_members.size() == 0)
        _channel_creator.insert(std::make_pair<int, User*>(++i, user)); //put in creator map

    _channel_members.insert(std::make_pair<int, User*>(++j, user)); //add in member map



    //server->_channels<
    // for (std::map<int, User*>::iterator it= this->_users.begin(); it != this->_users.end(); it++)
    // {

    // }


}

// void		Channel::deleteUser(User *user){
//      for (std::map<int, User*>::iterator it = _channel_members.begin(); it != _channel_members.end(); i++t)
//         if (*it->second->getNickname() == user->getNickname())
//             _channel_members.erase(it);
    
//     deleteChannel();

// }


// void Channel::setMode(const std::string mode, const User &user1, const User &user2) { if (user1->op == false || user1->creator == false) return ;

// // O - give "channel creator" status;
// if (mode == 'O')
// {
//     if (user1->creator == true)
//         user2->creator ==true;
// }
// // Ban (+b)
// if (mode == 'b')
//     deleteUser(user2);
// // INVITE  - Invite a client to an invite-only channel (mode +i)
// if (mode == 'i')
//     addUser(user2);
// //     o - give/take channel operator privilege;
// if (mode == 'o')
// {
//     if (user2->op == true)
//         user2->op == false;
//     else
//         user2->flag == true;
// }
// //     l - set/remove the user limit to channel;
// if (mode == 'l')
//     _maxUsers = 10^1000000;
// }




