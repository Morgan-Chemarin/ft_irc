#include "CommandInvite.hpp"
#include "Server.hpp"

void CommandInvite::execute(Server& server, Client& client, const IRCPrompt& prompt)
{
	if (prompt.args.size() < 2)
	{
		server.sendMessage(client.getFd(), MessageBuilder("461")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam("INVITE")
			.setContent("Not enough parameters"));
		return;
	}

	std::string targetNick = prompt.args[0];
	std::string channelName = prompt.args[1];

	Client *targetClient = server.getClientByNickname(targetNick);
	if (targetClient == NULL)
	{
		server.sendMessage(client.getFd(), MessageBuilder("401")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(targetNick)
			.setContent("No such nick/channel"));
		return;
	}
	
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL)
	{
		server.sendMessage(client.getFd(), MessageBuilder("403")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(channelName)
			.setContent("No such channel"));
		return;
	}
	if (!channel->hasMember(client.getFd()))
	{
		server.sendMessage(client.getFd(), MessageBuilder("442")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(channelName)
			.setContent("You're not on that channel"));
		return;
	}
 
	if (channel->getInviteOnly() && !channel->isOperator(client.getFd()))
	{
		server.sendMessage(client.getFd(), MessageBuilder("482")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(channelName)
			.setContent("You're not channel operator"));
		return;
	}

	if (channel->hasMember(targetClient->getFd()))
	{
		server.sendMessage(client.getFd(), MessageBuilder("443")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(targetNick)
			.setParam(channelName)
			.setContent("is already on channel"));
		return;
	}

	channel->addInvite(targetNick);

	server.sendMessage(client.getFd(), MessageBuilder("341")
		.setPrefix("ircserv")
		.setParam(client.getNickname())
		.setParam(targetNick)
		.setParam(channelName));

	server.sendMessage(targetClient->getFd(), MessageBuilder("INVITE")
		.setPrefix(client.getPrefix())
		.setParam(targetNick)
		.setContent(channelName));
}