#include "CommandNick.hpp"
#include "Server.hpp"

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

	client.setNickname(clientNickname);
	server.checkRegistration(client); // verifier si le processus d'enregistrement est fini avec fonction check
}