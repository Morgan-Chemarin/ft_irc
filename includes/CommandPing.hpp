#ifndef COMMANDPING_HPP
#define COMMANDPING_HPP

#include "ACommand.hpp"

class CommandPing : public ACommand
{
	public:
		virtual void execute(Server& server, Client& client, const IRCPrompt& prompt);

};

#endif