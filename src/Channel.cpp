#include "../includes/Channel.hpp"
#include "../includes/User.hpp"


Channel::Channel() {};    
Channel::Channel(std::string channel_name) {setName(channel_name); }
Channel::~Channel() {}

void        Channel::setName(const std::string channel_name){this->_name = channel_name;}
void		Channel::setUserStatus(int status) {this->_user_status = status;}
std::string Channel::getName() const {return this->_name;}
int         Channel::getUserStatus() {return this->_user_status;}





void		Channel::addUser(User *user){
    if (_channel_operators.size() == 0)
    {
        user->setNicknameOP("@" + user->getNickname());
        std::cout << " PUSH BACK OP ADD USER" << std::endl;
        _channel_operators.push_back(user);
    }
     _channel_members.push_back(user); 

}


void		Channel::deleteUser(User *user){
    std::cout << " HERE IN DELETE USER" << std::endl;
    std::cout << this->_channel_operators.size() << ": size channel operators" << std::endl;
    for (std::vector<User*>::iterator it = this->_channel_operators.begin(); it != this->_channel_operators.end(); it++)
    {
        std::cout << "usernickname: " << user->getNickname() << "|" << (*it)->getNickname() << std::endl;
        if ((*it)->getNickname() == user->getNickname())
        {
            std::cout << " HERE IN DELETE USER" << std::endl;
            _channel_operators.erase(it);
            break;
        }
    }
    for (std::vector<User*>::iterator it = this->_channel_members.begin(); it != this->_channel_members.end(); it++)
    {
        if ((*it)->getNickname() == user->getNickname())
        {
            std::cout << " HERE IN DELETE USER" << std::endl;
            _channel_members.erase(it);
            break ;
        }
    }
}

//user_op: user that typed the command
void    Channel::giveTakeOpPrivileges( User *user_not_op, User *user_op, std::string mode){
    if ( mode == "+o")
    {
        for (std::vector<User*>::iterator it = this->_channel_operators.begin(); it != this->_channel_operators.end(); it++)
        {
            if ((*it)->getNickname() == user_not_op->getNickname())
            {
                std::cout << "User " << user_not_op->getNickname() << " is already is already an Operator!\n";
                return ;
            }
            if (this->_channel_operators.end() - (it) == 1)//it means after we go out of the loop, we are at lat elem
                (user_not_op)->setNicknameOP("@"+(user_not_op)->getNickname());
        }
        this->_channel_operators.push_back(user_not_op);
    }
    else if (mode == "-o")
    {
         for (std::vector<User*>::iterator it = this->_channel_operators.begin(); it != this->_channel_operators.end(); it++)
        {
            if ((*it)->getNickname() == user_not_op->getNickname())
            {
                (user_not_op)->setNicknameOP("");
                _channel_operators.erase(it);
                break ;
            }
        }
    }

	for(std::vector<User*>::iterator it2 = _channel_operators.begin(); it2 != _channel_operators.end(); it2++)
	{
		std::cout << "#NICK: " << (*it2)->getNickname() << "\n";
    }

    user_op->getNickname();


}
