#include "CommandTopic.hpp"
#include "Server.hpp"

void	CommandTopic::readTopic(Server &server, Client &client, Channel *channel, const std::string &channelName)
{
	if (channel->getTopic().empty())
	{
		server.sendMessage(client.getFd(), MessageBuilder("331")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(channelName)
			.setContent("No topic is set"));
	}
	else
	{
		server.sendMessage(client.getFd(), MessageBuilder("332")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(channelName)
			.setContent(channel->getTopic()));
	}
}

void	CommandTopic::writeTopic(Server &server, Client &client, Channel *channel, const std::string &channelName, const std::string &topic)
{
	// Condition pour savoir si le client est moderateur
	if (!channel->isOperator(client.getFd()) || channel->getTopicProtected())
	{
		server.sendMessage(client.getFd(), MessageBuilder("482")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(channelName)
			.setContent("You're not channel operator"));
		return ;
	}
	channel->setTopic(topic);
	MessageBuilder topicMsg("TOPIC");
	topicMsg.setPrefix(client.getPrefix())
			.setParam(channelName)
			.setContent(topic);
	std::map<int, Client*> members = channel->getMembers();
	for (std::map<int, Client*>::iterator it = members.begin(); it != members.end(); ++it)
		server.sendMessage(it->first, topicMsg);
}

// cette commande permet de donner le topic du channel si elle a qu'un argument et si elle
// en a deux elle le modifie

void	CommandTopic::execute(Server& server, Client& client, const IRCPrompt& prompt)
{
	if (prompt.args.empty())
	{
		server.sendMessage(client.getFd(), MessageBuilder("461")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam("TOPIC")
			.setContent("Not enough parameters"));
		return ;
	}
	std::string channelName = prompt.args[0];
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL)
	{
		server.sendMessage(client.getFd(), MessageBuilder("403")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(channelName)
			.setContent("Not such channel"));
		return ;
	}
	if (!channel->hasMember(client.getFd()))
	{
		server.sendMessage(client.getFd(), MessageBuilder("442")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(channelName)
			.setContent("You're not on that channel"));
		return ;
	}
	if (prompt.args.size() == 1)
		readTopic(server, client, channel, channelName);
	else
		writeTopic(server, client, channel, channelName, prompt.args[1]);
}