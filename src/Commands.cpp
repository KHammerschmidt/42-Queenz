#include "../includes/Commands.hpp"

Commands::Commands() {}
Commands::~Commands() {}

// void Commands::getCommand(const std::string& cmd)
// {
// 	switch(getCommandID(cmd))
// 	{
// 		case PRIVMSG_PRIV:
// 			send_privmsg_priv();
// 			break ;
// 		case PRIVMSG_CHANNE:
// 			send_privmsg_channel();
// 			break;
// 		case REGISTER:
// 			register_new_user();
// 		default:
// 			invalid_cmd();
// 			break;
// 	}
// }

// void send_privmsg_priv() { return; }
// void send_privmsg_channel() { return; }
// void registe_new_user() { return; }
// void invalid_cmd() { return; }


// int Commands::getCommandID(const std::string& cmd)
// {
// 	if (cmd.compare("PRIVMSG"))
// 		return PRIVMSG_PRIV;
// 	else if (cmd.compare("PRIVMSG#"))
// 		return PRIVMSG_CHANNEL;
// 	else if (cmd.compare("NICK"))
// 		return REGISTER;
// 		// ....
// }