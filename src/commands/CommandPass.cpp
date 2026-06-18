#include "CommandPass.hpp"
#include "Server.hpp"

void CommandPass::execute(Server& server, Client& client, const IRCPrompt& prompt)
{
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
	if (prompt.args.empty())
	{
		server.sendMessage(client.getFd(), MessageBuilder("461")
			.setPrefix("ircserv")
			.setParam(target)
			.setParam("PASS")
			.setContent("Not enough parameters"));
		return ;
	}
	if (prompt.args[0] != server.getPassword())
	{
		server.sendMessage(client.getFd(), MessageBuilder("464")
			.setPrefix("ircserv")
			.setParam(target)
			.setContent("Password incorrect"));
		return ;
	}
	client.setHasPassword(true);
}