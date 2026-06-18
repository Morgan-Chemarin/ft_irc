#include "CommandNick.hpp"
#include "Server.hpp"

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
		server.sendMessage(client.getFd(), MessageBuilder("431")
			.setPrefix("ircserv")
			.setParam(target)
			.setContent("No nickname given"));
		return ;
	}
	std::string clientNick = prompt.args[0];
	char first = clientNick[0];
	if ((!std::isalpha(first) && std::string("[]\\^_`{|}").find(first) == std::string::npos) || clientNick.size() > 9)
	{
		server.sendMessage(client.getFd(), MessageBuilder("432")
			.setPrefix("ircserv")
			.setParam(target)
			.setParam(clientNick)
			.setContent("Erroneous nickname"));
		return ;
	}
	for (size_t i = 1; i < clientNick.size(); ++i)
	{
		char c = clientNick[i];
		if (!std::isalpha(c) && !std::isdigit(c) && std::string("[]\\^_`{|}-").find(c) == std::string::npos)
		{
			server.sendMessage(client.getFd(), MessageBuilder("432")
				.setPrefix("ircserv")
				.setParam(target)
				.setParam(clientNick)
				.setContent("Erroneous nickname"));
			return ;
		}
	}
	
	std::map<int, Client>& all_clients = server.getClients(); 
	for (std::map<int, Client>::iterator it = all_clients.begin() ; it != all_clients.end(); ++it)
	{
		if (it->second.getNickname() == clientNick)
		{
			if (it->first == client.getFd())
				continue ;

			server.sendMessage(client.getFd(), MessageBuilder("433")
			.setPrefix("ircserv")
			.setParam(target)
			.setParam(clientNick)
			.setContent("Nickname is already in use"));
			return ;
		}
	}
	
	std::string oldPrefix = client.getPrefix();
	client.setNickname(clientNick);

	server.sendMessage(client.getFd(), MessageBuilder("NICK")
		.setPrefix(oldPrefix)
		.setContent(clientNick));

	std::set<int> fdsToSend;
	
	const std::map<std::string, Channel>& channels = server.getChannels();
	for (std::map<std::string, Channel>::const_iterator itChan = channels.begin(); itChan != channels.end(); ++itChan)
	{
		if (itChan->second.hasMember(client.getFd()))
		{
			const std::map<int, Client*>& members = itChan->second.getMembers();
			for (std::map<int, Client*>::const_iterator itMem = members.begin(); itMem != members.end(); ++itMem)
			{
				if (itMem->first != client.getFd())
					fdsToSend.insert(itMem->first);
			}
		}
	}

	for (std::set<int>::iterator itFd = fdsToSend.begin(); itFd != fdsToSend.end(); ++itFd)
	{
		server.sendMessage(*itFd, MessageBuilder("NICK")
			.setPrefix(oldPrefix)
			.setContent(clientNick));
	}

	server.checkRegistration(client);
}