#ifndef COMMANDMODE_HPP
#define COMMANDMODE_HPP

#include "ACommand.hpp"

class CommandMode : public ACommand
{
	public:
		virtual void execute(Server& server, Client& client, const IRCPrompt& prompt);

};

#endif