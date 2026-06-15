#include "CommandPass.hpp"
#include "Server.hpp"

// Cette fonction regarde si le client est deja enregistre, si l'arg n'est pas vide et si le password est correct
// suite a cela il va changer la variable _hasPassword en true. Dans le cas ou une erreur survenait le programme enverrai
// un message specifique au client

void CommandPass::execute(Server& server, Client& client, const IRCPrompt& prompt)
{
	if (client.getIsRegistered())
	{
		server.sendMessage(client.getFd(), "462", ":You may not reregister");
		return ;
	}
	if (prompt.args.empty())
	{
		server.sendMessage(client.getFd(), "461", ":PASS :Not enough parameters");
		return ;
	}
	if (prompt.args[0] != server.getPassword())
	{
		server.sendMessage(client.getFd(), "464", ":Password incorrect");
		return ;
	}
	client.setHasPassword(true);
}