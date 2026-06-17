#include "CommandPing.hpp"
#include "Server.hpp"

// Cette commande sert a verifier si la connexion est toujours opperante, elle ping et attend une reponse pong avec le meme token envoye

void CommandPing::execute(Server& server, Client& client, const IRCPrompt& prompt)
{
	if (prompt.args.empty())
	{
		server.sendMessage(client.getFd(), MessageBuilder("409")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setContent("No origin specified"));
		return;
	}

	// on renvoi pong avec le token uqi laccompagne
	server.sendMessage(client.getFd(), MessageBuilder("PONG")
		.setPrefix("ircserv")
		.setParam("ircserv")
		.setContent(prompt.args[0]));
}