#include "CommandPrivmsg.hpp"
#include "Server.hpp"

void	CommandPrivmsg::sendToChannel(Server& server, Client& client, const std::string &target, const MessageBuilder &msgBuilder)
{
	Channel *channel = server.getChannel(target);
	if (channel == NULL)
	{
		server.sendMessage(client.getFd(), MessageBuilder("401")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(target)
			.setContent("No such nick/channel"));
		return ;
	}
	if (!channel->hasMember(client.getFd()))
	{
		server.sendMessage(client.getFd(), MessageBuilder("404")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(target)
			.setContent("Cannot send to channel"));
		return ;
	}
	std::map<int, Client*> members = channel->getMembers();
	for (std::map<int, Client*>::iterator it = members.begin(); it != members.end(); ++it)
	{
		if (it->first != client.getFd())
			server.sendMessage(it->first, msgBuilder);
	}
	
}

void	CommandPrivmsg::sendToUser(Server& server, Client& client, const std::string &target, const MessageBuilder &msgBuilder)
{
	Client *targetClient = server.getClientByNickname(target);
	if (targetClient == NULL)
	{
		server.sendMessage(client.getFd(), MessageBuilder("401")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(target)
			.setContent("No such nick/channel"));
		return ;
	}
	server.sendMessage(targetClient->getFd(), msgBuilder);
}

void	CommandPrivmsg::execute(Server &server, Client &client, const IRCPrompt &prompt)
{
	if (prompt.args.empty())
	{
		server.sendMessage(client.getFd(), MessageBuilder("411")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setContent("No recipient given (PRIVMSG)"));
		return ;
	}
	if (prompt.args.size() < 2)
	{
		server.sendMessage(client.getFd(), MessageBuilder("412")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setContent("No text to send"));
		return ;
	}
	std::string	target = prompt.args[0];
	std::string	text = prompt.args[1];
	MessageBuilder	msgBuilder("PRIVMSG");
	msgBuilder.setPrefix(client.getPrefix())
		.setParam(target)
		.setContent(text);
	if (target[0] == '#')
		sendToChannel(server, client, target, msgBuilder);
	else
		sendToUser(server, client, target, msgBuilder);
}