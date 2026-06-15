#ifndef COMMANDJOIN_HPP
#define COMMANDJOIN_HPP

#include "ACommand.hpp"

class CommandJoin : public ACommand
{
	public:
		virtual void execute(Server& server, Client& client, const IRCPrompt& prompt);

};

#endif