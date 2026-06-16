#ifndef COMMANDTOPIC_HPP
#define COMMANDTOPIC_HPP

#include "ACommand.hpp"

class CommandTopic : public ACommand
{
	public:
		virtual void execute(Server& server, Client& client, const IRCPrompt& prompt);

};

#endif