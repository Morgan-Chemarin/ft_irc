#ifndef COMMANDPRIVMSG_HPP
#define COMMANDPRIVMSG_HPP

#include "ACommand.hpp"
#include "MessageBuilder.hpp"

class CommandPrivmsg : public ACommand
{
	public:
		virtual void execute(Server& server, Client& client, const IRCPrompt& prompt);
	private:
		void	sendToChannel(Server& server, Client& client, const std::string &target, const MessageBuilder &msgBuilder);
		void	sendToUser(Server& server, Client& client, const std::string &target, const MessageBuilder &msgBuilder);

};

#endif