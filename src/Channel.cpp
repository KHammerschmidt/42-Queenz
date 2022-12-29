#include "../includes/Channel.hpp"
#include "../includes/User.hpp"

// to create a channel -> in USER: if (user parse tipes #join) -> Channel:createChannel (with static method); if doesnt work, delete createChannel and in User do new Channel()

Channel::Channel() {};    //Kathi: I don't think a default constructor makes sense for Channels
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
    {
        user->setNicknameOP();
        _channel_operators.push_back(user); //put in creator map
    }
    else
        _channel_members.push_back(user); //add in member map

}


void		Channel::deleteUser(User *user){
    //make var server and delete also from struct in server
    //  for (std::multimap<std::string, User*>::iterator it = server->_channel_users.begin(); it != _channel_members.end(); i++t)
    //     if (*it->second->getNickname() == user->getNickname())
    //         _channel_members.erase(it);

    for (std::vector<User*>::iterator it = this->_channel_operators.begin(); it != this->_channel_operators.end(); it++)
    {
        if ((*it)->getNickname() == user->getNickname())
            _channel_operators.erase(it);
    }
    for (std::vector<User*>::iterator it = this->_channel_members.begin(); it != this->_channel_members.end(); it++)
    {
        if ((*it)->getNickname() == user->getNickname())
            _channel_members.erase(it);
    }

    // if (this->_channel.members.size() == 0)
        // delete *this;
}



bool    Channel::returnPrivilegesStatus(std::string user_nickname)
{
    for (std::vector<User*>::iterator it = this->_channel_operators.begin(); it != this->_channel_operators.end(); it++)
    {
        if ((*it)->getNickname() == user_nickname)
            return true;
    }

    //just creator can make operators or also operators?
    for (std::vector<User*>::iterator it = this->_channel_operators.begin(); it != this->_channel_operators.end(); it++)
    {
        if ((*it)->getNickname() == user_nickname)
            return true;
    }
    return false;
}

//by mode="-o" => both users are OP (user_not_op is the one should be processed)
void    Channel::giveTakeOpPrivileges( User *user_not_op, std::string nickname_user_op, std::string mode){

    if ( mode == "+o" && returnPrivilegesStatus(nickname_user_op) == true)
        _channel_operators.push_back(user_not_op);
    else if (mode == "-o" && returnPrivilegesStatus(user_not_op->getNickname()) == true
                            && returnPrivilegesStatus(nickname_user_op) == true)
    {

        	for (std::vector<User*>::iterator it = _channel_operators.begin(); it != _channel_operators.end(); it++)
        		if ((*it)->getNickname().compare(user_not_op->getNickname()) == 0)
                    _channel_operators.erase(it);
    }
    //implement before in  command before command executed: check is is op looping over Channel::returnPrivilegesStatus; if true, set @before Nickname
}






// void Channel::print_vector(std::vector<std::string> vctr)
// {
// 	std::vector<std::string>::iterator iter;
// 	for (iter = vctr.begin(); iter != vctr.end(); iter++)
// 		std::cout << *iter << std::endl;
// }
