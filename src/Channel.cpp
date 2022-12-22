#include "../includes/Channel.hpp"
#include "../includes/User.hpp"

// to create a channel -> in USER: if (user parse tipes #join) -> Channel:createChannel (with static method); if doesnt work, delete createChannel and in User do new Channel()

Channel::Channel() {};
Channel::Channel(std::string channel_name) {setName(channel_name); }//i = 0; j = 0; k = 0;}
Channel::~Channel() {}

void        Channel::setName(const std::string channel_name){this->_name = channel_name;}
void		Channel::setUserStatus(int status) {this->_user_status = status;}
std::string Channel::getName() const {return this->_name;}
int         Channel::getUserStatus() {return this->_user_status;}
// std::map<int, User*>  Channel::getChannelMembers(){return this->_channel_members;}
// std::map<int, User*>  Channel::getChannelOperators(){return this->_channel_operators;}
// std::map<int, User*>  Channel::getChannelCreator(){return this->_channel_creator;}



void        Channel::deleteChannel()
{
   // if (_channel_members.size() == 0)
    //    delete this; //suicide
}


// void        Channel::setTopic(const std::string topic){this->_topic = topic;}
// std::string Channel::getTopic() const{return this->_topic;}

// void Channel::changeTopic(const std::string newTopic) {
//     setTopic(newTopic); 
//     std::cout << "The topic of the channel was updatet!" << std::endl;
// }


void		Channel::addUser(User *user){//, Server *server){

    if (_channel_members.size() == 0)
        _channel_creator.push_back(user); //put in creator map
    else
        _channel_members.push_back(user); //add in member map

}


// void		Channel::deleteUser(User *user){
//      for (std::map<int, User*>::iterator it = _channel_members.begin(); it != _channel_members.end(); i++t)
//         if (*it->second->getNickname() == user->getNickname())
//             _channel_members.erase(it);
    
//     deleteChannel();

// }


// void Channel::setMode(const std::string mode, User *user_op,  User *user_not_op) { 
    
//     if (returnPrivilegesStatus(user_op->getNickname()) != true)
//         return ;

//     // O - give "channel creator" status;
//     if (mode == 'O')
//     {
//         if (user1->creator == true)
//             user2->creator ==true;
//     }
//     // Ban (+b)
//     if (mode == 'b')
//         deleteUser(user2);
//     // INVITE  - Invite a client to an invite-only channel (mode +i)
//     if (mode == 'i')
//         addUser(user2);
//     //     o - give/take channel operator privilege;
//     if (mode == 'o')
//     {
//         if (user2->op == true)
//             user2->op == false;
//         else
//             user2->flag == true;
//     }

// }

bool    Channel::returnPrivilegesStatus(std::string user_nickname)
{
    user_nickname = " ";
    return true;//to let it compile :)

    // for (std::map<int,User*>::iterator it = this->_channel_operators.begin(); it != this->_channel_operators.end(); it++)
    // {
    //     if ((*it).second->getNickname() == user_nickname)
    //         return true;
    // }

    //     for (std::map<int,User*>::iterator it = this->_channel_creator.begin(); it != this->_channel_creator.end(); it++)
    // {
    //     if ((*it).second->getNickname() == user_nickname)
    //         return true;
    // }
    // return false;
}

//need 2 users as input? or reicht nickname of einen?
void    Channel::giveOpPrivileges( User *user_not_op, std::string nickname_user_op){
    user_not_op->getNickname();/*to let it compile :)*/
    nickname_user_op = " ";
    // int key;

    // if (returnPrivilegesStatus(nickname_user_op) == true)
    // {
    //     _channel_operators.insert(std::make_pair<int, User*>(++j, user_not_op)); //put in creator map
    //     for (std::map<int,User*>::iterator it = this->_channel_members.begin(); it != this->_channel_members.end(); it++)
    //     {
    //         key = (*it).first;
    //         if ((*it).second->getNickname() == user_not_op->getNickname())
    //             _channel_operators.erase(key);
    //     }
    // }
}






// void Channel::print_vector(std::vector<std::string> vctr)
// {
// 	std::vector<std::string>::iterator iter;
// 	for (iter = vctr.begin(); iter != vctr.end(); iter++)
// 		std::cout << *iter << std::endl;
// }
