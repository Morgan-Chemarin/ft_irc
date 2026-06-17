#include "CommandJoin.hpp"
#include "Server.hpp"

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

	if (prompt.args[0] == "0")
	{
		//! PART sur tous les channels 
		return;
	}

	// le premier arguement ou ya le nom du channel peut etre composé de plueirus channel 
	// ex = JOIN room1,room2...
	std::vector<std::string> channels;
	std::vector<std::string> keys;

	// om separe les channels dans le vector
	std::stringstream ssChan(prompt.args[0]);
	std::string tokenChan;
	while (std::getline(ssChan, tokenChan, ','))
	{
		channels.push_back(tokenChan);
	}

	// pareil pour les mots de passe (le deuxieme param) 
	if (prompt.args.size() > 1)
	{
		std::stringstream ssKeys(prompt.args[1]);
		std::string tokenKey;
		while (std::getline(ssKeys, tokenKey, ','))
			keys.push_back(tokenKey);
	}

	// on bouvle sur outs les channels a rejoindre
	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string channelName = channels[i];
		std::string key = "";

		if (i < keys.size())
			key = keys[i];

		// check le nom du channel doit commencer par un caractere speciale/pas despace
		if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&' && channelName[0] != '+' && channelName[0] != '!'))
		{
			server.sendMessage(client.getFd(), MessageBuilder("476")
				.setPrefix("ircserv")
				.setParam(client.getNickname())
				.setParam(channelName)
				.setContent("Bad Channel Mask"));
			continue ;
		}

		Channel* chan = server.getChannel(channelName);
		// si le channel n'existe pas on le créé, càd on créé une nouvelle instance 
		// et on lajoute a la map des channels de Server a la key = nom du channel
		if (chan == NULL)
		{
			server.addChannel(channelName);
			chan = server.getChannel(channelName);
			std::cout << "Le client a cree la room " << channelName << std::endl;
			chan->addOperator(client.getFd()); // pou mettre le createur operator
		}
		else
		{
			// check si le fd nest pas deja dans le channel
			if (chan->hasMember(client.getFd()))
				continue ;

			// si le channel est en inviteOnly on verifie si le client est invite
			if (chan->getInviteOnly() && !chan->isInvited(client.getNickname()))
			{
				server.sendMessage(client.getFd(), MessageBuilder("473")
					.setPrefix("ircserv")
					.setParam(client.getNickname())
					.setParam(channelName)
					.setContent("Cannot join channel (+i)"));
				continue ;
			}

			// si le channel a besoin dun mdp, si cest pas le bon 
			if (chan->hasKey() && chan->getKey() != key)
			{
				server.sendMessage(client.getFd(), MessageBuilder("475")
					.setPrefix("ircserv")
					.setParam(client.getNickname())
					.setParam(channelName)
					.setContent("Cannot join channel (+k)"));
				continue ;
			}

			// verifie sil y a une limite de personne sur le channel
			if (chan->hasLimit() && chan->getMembers().size() >= (size_t)chan->getLimitUsers())
			{
				server.sendMessage(client.getFd(), MessageBuilder("471")
					.setPrefix("ircserv")
					.setParam(client.getNickname())
					.setParam(channelName)
					.setContent("Cannot join channel (+l)"));
				continue ;
			}
		}

		// on recupere le pointeur du client pour lajouter dans le channel ( que lon vient de creer ou non )
		chan->addMember(&client);
		// on supprime linvitation quqnd le client rejoint
		chan->removeInvite(client.getNickname());
		std::cout << "Le client " << client.getNickname() << " (fd " << client.getFd() 
				<< ") a rejoint le channel " << channelName << std::endl;

		// broadcast pour envoyer a tous les membres du channel linf oque quelquun a rejoint
		// :Momo!user@host JOIN #channel
		std::string userPrefix = client.getPrefix();
		const std::map<int, Client*>& members = chan->getMembers();

		// on broadcast a TOUT le monde, celui qui rejoint aussi
		for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			server.sendMessage(it->first, MessageBuilder("JOIN")
				.setPrefix(userPrefix)
				.setParam(channelName));
		}

		// envoyer le topic du channel sil en a un
		if (chan->getTopic().empty())
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
				.setContent(chan->getTopic()));
		}

		// envoyer la liste les membres du channel
		std::string userList = "";
		for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			if (chan->isOperator(it->second->getFd()))
				userList += "@";
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
}