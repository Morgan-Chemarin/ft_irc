#include "CommandPrivmsg.hpp"
#include "Server.hpp"

// Si il y a un # devant la cible lors d'un appel de PRIVMSG alors on doit envoye le message a un channel

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
	// on verifie que le client qui veut envoyer un message sur le channel fait bien partie de celui-ci
	if (!channel->hasMember(client.getFd()))
	{
		server.sendMessage(client.getFd(), MessageBuilder("404")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(target)
			.setContent("Cannot send to channel"));
		return ;
	}
	// on renseigne tout les membres du channel dans une map puis ensuite grace a cette map on leur envoie tous le message
	std::map<int, Client*> members = channel->getMembers();
	for (std::map<int, Client*>::iterator it = members.begin(); it != members.end(); ++it)
	{
		// on verifie bien que le fd n'est pas celui du client qui envoie le message car il ne dois pas le recevoir
		if (it->first != client.getFd())
			server.sendMessage(it->first, msgBuilder);
	}
	
}

// cette fonction permet d'envoyer un message au client cible 

void	CommandPrivmsg::sendToUser(Server& server, Client& client, const std::string &target, const MessageBuilder &msgBuilder)
{
	Client *targetClient = server.getClientByNickname(target); // on recherche notre client cible dans _clients grace au nickname
	// si le client n'existe pas on renvoie un message d'erreur
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
	// on verifie que le client ai bien donne une cible
	if (prompt.args.empty())
	{
		server.sendMessage(client.getFd(), MessageBuilder("411")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setContent("No recipient given (PRIVMSG)"));
		return ;
	}
	// on verifie que le client ai bien donne un texte a envoye
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
	// On parametre le message qui va etre envoye
	MessageBuilder	msgBuilder("PRIVMSG");
	msgBuilder.setPrefix(client.getPrefix())
		.setParam(target)
		.setContent(text);
	if (target[0] == '#')
		sendToChannel(server, client, target, msgBuilder);
	else
		sendToUser(server, client, target, msgBuilder);
}