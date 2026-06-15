#include "CommandJoin.hpp"
#include "Server.hpp"
#include <iostream>

void CommandJoin::execute(Server& server, Client& client, const IRCPrompt& prompt)
{
	if (prompt.args.empty()) 
	{
        std::cout << "ERR_NEEDMOREPARAMS pour le fd " << client.getFd() << "\n";
        return;
    }

	// le premier arguement ou ya le nom du channel peut etre composé de plueirus channel 
	// ex = JOIN room1,room2...
	//? pour linstant je gere que un channel en arg je ferai demain
	std::string channelName = prompt.args[0];

	Channel* chan = server.getChannel(channelName);
	// si le channel n'existe pas on le créé, càd on créé une nouvelle instance 
	// et on lajoute a la map des channels de Server a la key = nom du channel
	if (chan == NULL) {
		server.addChannel(channelName); // Plus de accès direct à _channels !
		chan = server.getChannel(channelName); 

		//? TEST
		std::cout << "Le client a cree la room " << channelName << std::endl;
	}

	//! check si le fd nest pas deja dans le channel

	// on recupere le pointeur du client pour lajouter dans le channel ( que lon vient de creer ou non )
	chan->addMember(&client);

	std::cout << "Le client " << client.getNickname() << " (fd " << client.getFd() 
              << ") a rejoint le channel " << channelName << std::endl;

	//? envoyer les messages IRC avec send
	// RPL_JOIN, RPL_NAMREPLY, ERR_NEEDMOREPARAMS

	// sendMsg(fd, msg) ?;
}