#include "CommandJoin.hpp"
#include "Server.hpp"
#include <iostream>

void CommandJoin::execute(Server& server, Client& client, const IRCPrompt& prompt)
{
	if (prompt.args.empty()) 
	{
		server.sendMessage(client.getFd(), MessageBuilder("461")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam("JOIN")
			.setContent("Not enough parameters"));
		return;
	}

	// le premier arguement ou ya le nom du channel peut etre composé de plueirus channel 
	// ex = JOIN room1,room2...
	//! pour linstant je gere que un channel en arg je ferai demain
	std::string channelName = prompt.args[0];

	std::cout << channelName << std::endl;
	// check le nom du channel doit commencer par un caractere speciale/pas despace
	if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&' && channelName[0] != '+' && channelName[0] != '!'))
	{
		server.sendMessage(client.getFd(), MessageBuilder("476")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(channelName)
			.setContent("Bad Channel Mask"));
		return;
	}

	Channel* chan = server.getChannel(channelName);
	// si le channel n'existe pas on le créé, càd on créé une nouvelle instance 
	// et on lajoute a la map des channels de Server a la key = nom du channel
	if (chan == NULL)
	{
		server.addChannel(channelName);
		chan = server.getChannel(channelName);
		std::cout << "Le client a cree la room " << channelName << std::endl; // ?test
	}

	// check si le fd nest pas deja dans le channel
	if (chan->hasMember(client.getFd()))
		return;

	// on recupere le pointeur du client pour lajouter dans le channel ( que lon vient de creer ou non )
	chan->addMember(&client);
	std::cout << "Le client " << client.getNickname() << " (fd " << client.getFd() 
			  << ") a rejoint le channel " << channelName << std::endl;

	// broadcast pour envoyer a tous les membres du channel linf oque quelquun a rejoint
    const std::map<int, Client*>& members = chan->getMembers();
	// on droadcats a TOUT le monde, celui qui rejoint aussi
	for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
    {
        server.sendMessage(it->first, MessageBuilder("JOIN")
            .setPrefix(client.getPrefix())
            .setParam(channelName));
    }

	// envoyer le topic du channel sil en a un ( //! 331 sil en a pas mais pour linstant pn fait comme on peut)
	server.sendMessage(client.getFd(), MessageBuilder("332")
        .setPrefix("ircserv")
        .setParam(client.getNickname())
        .setParam(channelName)
        .setContent("Bienvenue sur " + channelName));

	// envoyer la liste les membres du channel
	std::string userList = "";
    for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		// ajouter un @ pour que le client comprennent qui est operator (demain avec mode)
        userList += it->second->getNickname() + " ";
	}

	// del le derniere espace pas beau
	if (!userList.empty())
        userList.erase(userList.size() - 1);

	// pour envoyer la liste des membres sur le channel
	server.sendMessage(client.getFd(), MessageBuilder("353")
        .setPrefix("ircserv")
        .setParam(client.getNickname())
        .setParam("=") // ce vilain egale cets pour dire que le channel est public 
        .setParam(channelName)
        .setContent(userList));

    // obligatoir pour dire quon a fini de donner la liste de mebmres
    server.sendMessage(client.getFd(), MessageBuilder("366")
        .setPrefix("ircserv")
        .setParam(client.getNickname())
        .setParam(channelName)
        .setContent("End of /NAMES list"));
}

//? BROADCAST -> RPL_TOPIC ( 332 ou lautre 331 ) -> RPL_NAMEREPLY ( 353 ) -> RPL_ENDOFNAMES ( 366 )