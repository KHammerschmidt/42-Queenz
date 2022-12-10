//copy in deinen Teil von Command.cpp


void Command::invokeMessage(User *user)
{
	std::vector<std::string> commands_to_exec;

	size_t pos_end_command = user->buffer.find(MSG_END);
	if (pos_end_command == std::string::npos)			//no full command
		return ;

	while (pos_end_command != std::string::npos)
	{
		std::string tmp = user->buffer.substr(0, pos_end_command);						//create a substring until delimiter ("\r\n")
		std::cout << " This is the substring of the command: " << tmp << std::endl;		//test printing
		commands_to_exec.push_back(tmp);												// push at end of vector

		user->buffer = user->buffer.substr(pos_end_command, user->buffer.length());		//cut out first command
		pos_end_command = user->buffer.find(MSG_END);									//search for an additional command ("\r\n") would be present
	}
	execute_command(user, commands_to_exec);
}
