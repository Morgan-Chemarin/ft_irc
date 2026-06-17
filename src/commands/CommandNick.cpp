#include "CommandNick.hpp"
#include "Server.hpp"
#include <set> 

// Cette fonction permet au client de definir un nickname, mais avant tout le client doit toujours faire le password en premier.
// La fonction gere les doublons grace au for et a l'iterateur

void CommandNick::execute(Server& server, Client& client, const IRCPrompt& prompt)
{
	if (!client.getHasPassword())
		return ;
	std::string target;
	if (client.getNickname().empty())
		target = "*";
	else
		target = client.getNickname();
	if (prompt.args.empty())
	{
		server.sendMessage(client.getFd(), MessageBuilder("432")
			.setPrefix("ircserv")
			.setParam(target)
			.setContent("No nickname given"));
		return ;
	}
	
	std::string clientNickname = prompt.args[0];
	
	std::map<int, Client>& all_clients = server.getClients(); 
	for (std::map<int, Client>::iterator it = all_clients.begin() ; it != all_clients.end(); ++it)
	{
		if (it->second.getNickname() == clientNickname)
		{
			server.sendMessage(client.getFd(), MessageBuilder("433")
			.setPrefix("ircserv")
			.setParam(target)
			.setParam(clientNickname)
			.setContent("Nickname is already in use"));
			return ;
		}
	}
	
	std::string oldPrefix = client.getPrefix();
	client.setNickname(clientNickname);

	// on lenvoi a soit meme
	server.sendMessage(client.getFd(), MessageBuilder("NICK")
        .setPrefix(oldPrefix)
        .setContent(clientNickname));

	// set pour avoir que des fd unique
	std::set<int> fdsToSend;
	
	const std::map<std::string, Channel>& channels = server.getChannels();
    for (std::map<std::string, Channel>::const_iterator itChan = channels.begin(); itChan != channels.end(); ++itChan)
    {
		// si le client est dans le channel on va ajouter tous les fd du channel
        if (itChan->second.hasMember(client.getFd()))
        {
            const std::map<int, Client*>& members = itChan->second.getMembers();
            for (std::map<int, Client*>::const_iterator itMem = members.begin(); itMem != members.end(); ++itMem)
            {
				// on se compte pas sois meme
                if (itMem->first != client.getFd())
                    fdsToSend.insert(itMem->first);
            }
        }
    }

	// envoi du message nick a tous les fd selectionner
	for (std::set<int>::iterator itFd = fdsToSend.begin(); itFd != fdsToSend.end(); ++itFd)
    {
        server.sendMessage(*itFd, MessageBuilder("NICK")
            .setPrefix(oldPrefix)
            .setContent(clientNickname));
    }

	server.checkRegistration(client); // verifier si le processus d'enregistrement est fini avec fonction check
}