#include "CommandPrivmsg.hpp"
#include "Server.hpp"

// void	CommandPrivmsg::sendToChannel(Server& server, Client& client, const std::string &target, const MessageBuilder &msgBuider)
// {

// }

// cette fonction permet d'envoyer un message au client cible 

void	CommandPrivmsg::sendToUser(Server& server, Client& client, const std::string &target, const MessageBuilder &msgBuider)
{
	Client *targetClient = server.getClientWithNick(target); // on recherche notre client cible dans _clients grace au nickname
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
	server.sendMessage(targetClient->getFd(), msgBuider);
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
	msgBuilder.setPrefix(client.getNickname())
		.setParam(target)
		.setContent(text);
	if (target[0] == '#')
		//sendToChannel(server, client, target, msgBuilder);
		return ; // temporaire
	else
		sendToUser(server, client, target, msgBuilder);
}