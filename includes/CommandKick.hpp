#ifndef COMMANDKICK_HPP
#define COMMANDKICK_HPP

#include "ACommand.hpp"

class CommandKick : public ACommand
{
	public:
		virtual void execute(Server& server, Client& client, const IRCPrompt& prompt);

};

#endif