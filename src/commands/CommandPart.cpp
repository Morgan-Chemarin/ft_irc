#include "CommandPart.hpp"
#include "Server.hpp"

// cette commande sert a quitter un channel. Le serveur envoie un message comme quoi un client 
// a quitter le serveur. Si jamais le channel est vide le serveur supprime le channel.

void CommandPart::execute(Server& server, Client& client, const IRCPrompt& prompt)
{
	if (prompt.args.empty()) 
	{
		server.sendMessage(client.getFd(), MessageBuilder("461")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam("PART")
			.setContent("Not enough parameters"));
		return;
	}
	std::string channelName = prompt.args[0];
	std::string reason = "Leaving";
	if (prompt.args.size() > 1)
		reason = prompt.args[1];
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
	MessageBuilder partMsg("PART");
	partMsg.setPrefix(client.getPrefix())
			.setParam(channelName)
			.setContent(reason);
	std::map<int, Client*> members = channel->getMembers();
	for (std::map<int, Client*>::iterator it = members.begin(); it != members.end(); ++it)
		server.sendMessage(it->first, partMsg);
	channel->removeMember(client.getFd());
	channel->removeOperator(client.getFd());
	if (channel->getMembers().empty())
		server.removeChannel(channelName);
}