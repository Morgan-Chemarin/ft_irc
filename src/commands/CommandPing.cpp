#include "CommandPing.hpp"
#include "Server.hpp"

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
	server.sendMessage(client.getFd(), MessageBuilder("PONG")
		.setPrefix("ircserv")
		.setParam("ircserv")
		.setContent(prompt.args[0]));
}