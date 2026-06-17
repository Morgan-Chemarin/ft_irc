#ifndef COMMANDINVITE_HPP
#define COMMANDINVITE_HPP

#include "ACommand.hpp"

class CommandInvite : public ACommand
{
	public:
		virtual void execute(Server& server, Client& client, const IRCPrompt& prompt);

};

#endif