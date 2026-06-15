#include "CommandUser.hpp"
#include "Server.hpp"

// Cette fonction permet au client de definir son username. Comme pour checkNICK elle regarde en premier si 
// le client a rentrer le mdp puis elle regarde si la commande est au bon format et si tout est valide 
// elle enregistre l'username

void CommandUser::execute(Server& server, Client& client, const IRCPrompt& prompt)
{
	if (!client.getHasPassword())
		return ;
	std::string target;
	if (client.getNickname().empty())
		target = "*";
	else
		target = client.getNickname();
	if (client.getIsRegistered())
	{
		server.sendMessage(client.getFd(), MessageBuilder("462")
			.setPrefix("ircserv")
			.setParam(target)
			.setContent("You may not reregister"));
		return ;
	}
	if (prompt.args.size() < 4) // format client USER = USER <username> <hostname> <servername> <realname>
	{
		server.sendMessage(client.getFd(), MessageBuilder("461")
			.setPrefix("ircserv")
			.setParam(target)
			.setParam("USER")
			.setContent("Not enough parameters"));
		return ;
	}
	
	client.setUsername(prompt.args[0]);
	server.checkRegistration(client);
}