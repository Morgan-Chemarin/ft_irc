#include "Server.hpp"

void Server::cmdJoin(int fd, const IRCPrompt& prompt)
{
	if (prompt.args.empty())
	{
		std::cout << "pas darg\n"; //? envoyer ERR_NEEDMOREPARAMS
		return;
	}

	// le premier arguement ou ya le nom du channel peut etre composé de plueirus channel 
	// ex = JOIN room1,room2...
	//? pour linstant je gere que un channel en arg je ferai demain
	std::string channelName = prompt.args[0];

	// si le channel n'existe pas on le créé, càd on créé une nouvelle instance 
	// et on lajoute a la map des channels de Server a la key = nom du channel
	if (_channels.find(channelName) == _channels.end())
    {
        Channel newChannel(channelName);
        _channels[channelName] = newChannel;
    }

	//! check si le fd nest pas deja dans le channel

	// on recupere le pointeur du client pour lajouter dans le channel ( que lon vient de creer ou non )
	Client* currentClient = &_clients[fd];
    _channels[channelName].addMember(currentClient);

	std::cout << "Le client fd " << fd << " a rejoint le channel " << channelName << std::endl;

	//? envoyer les messages IRC avec send
	// RPL_JOIN, RPL_NAMREPLY, ERR_NEEDMOREPARAMS

	// sendMsg(fd, msg) ?;
}