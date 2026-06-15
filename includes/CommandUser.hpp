#ifndef COMMANDUSER_HPP
#define COMMANDUSER_HPP

#include "ACommand.hpp"

class CommandUser : public ACommand
{
	public:
		virtual void execute(Server& server, Client& client, const IRCPrompt& prompt);

};

#endif