#ifndef COMMANDTOPIC_HPP
#define COMMANDTOPIC_HPP

#include "ACommand.hpp"

class Channel;

class CommandTopic : public ACommand
{
	public:
		virtual void execute(Server& server, Client& client, const IRCPrompt& prompt);
	private:
		void readTopic(Server &server, Client &client, Channel *channel, const std::string &channelName);
		void writeTopic(Server &server, Client &client, Channel *channel, const std::string &channelName, const std::string &topic);
};

#endif