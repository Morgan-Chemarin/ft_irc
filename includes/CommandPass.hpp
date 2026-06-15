#ifndef COMMANDPASS_HPP
#define COMMANDPASS_HPP

#include "ACommand.hpp"

class CommandPass : public ACommand
{
	public:
		virtual void execute(Server& server, Client& client, const IRCPrompt& prompt);

};

#endif