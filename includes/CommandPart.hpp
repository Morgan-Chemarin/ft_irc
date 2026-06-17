#ifndef COMMANDPART_HPP
#define COMMANDPART_HPP

#include "ACommand.hpp"

class CommandPart : public ACommand
{
	public:
		virtual void execute(Server& server, Client& client, const IRCPrompt& prompt);

};

#endif