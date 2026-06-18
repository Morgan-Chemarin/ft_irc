#ifndef COMMANDNICK_HPP
#define COMMANDNICK_HPP

#include "ACommand.hpp"
#include <set> 

class CommandNick : public ACommand
{
	public:
		virtual void execute(Server& server, Client& client, const IRCPrompt& prompt);

};

#endif