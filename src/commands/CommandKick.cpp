#include "CommandKick.hpp"
#include "Server.hpp"

// cette commande permet d'exclure un client d'une room. Il faut etre moderateur pour pouvoir utilise cette commande

void CommandKick::execute(Server& server, Client& client, const IRCPrompt& prompt)
{
	if (prompt.args.size() < 2)
	{
		server.sendMessage(client.getFd(), MessageBuilder("461")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam("KICK")
			.setContent("Not enough parameters"));
		return ;
	}
	std::string	channelName = prompt.args[0];
	std::string	target = prompt.args[1];
	std::string reason = "Kicked by an operator.";
	if (prompt.args.size() > 2)
		reason = prompt.args[2];

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
	// Condition pour savoir si le client est moderateur
	if (!channel->isOperator(client.getFd()))
	{
		server.sendMessage(client.getFd(), MessageBuilder("482")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(channelName)
			.setContent("You're not channel operator"));
		return ;
	}
	Client *targetClient = server.getClientWithNick(target);
	// condition pour verifier si le client cible est sur le serveur et si il est dans le channel
	if (targetClient == NULL || !channel->hasMember(targetClient->getFd()))
	{
		server.sendMessage(client.getFd(), MessageBuilder("441")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(target)
			.setParam(channelName)
			.setContent("They aren't on that channel"));
		return ;
	}
	// apres toutes les verifications on construit le message, on l'envoie a tout les membres du channel et on kick la cible
	MessageBuilder kickMsg("KICK");
	kickMsg.setPrefix(client.getPrefix())
			.setParam(channelName)
			.setParam(target)
			.setContent(reason);
	std::map<int, Client*> members = channel->getMembers();
	for (std::map<int, Client*>::iterator it = members.begin(); it != members.end(); ++it)
		server.sendMessage(it->first, kickMsg);
	channel->removeMember(targetClient->getFd());
}