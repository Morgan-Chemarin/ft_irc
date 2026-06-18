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
		std::map<std::string, Channel> channels = server.getChannels();
		std::vector<std::string> channelsLeave;

		for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
		{
			if (it->second.hasMember(client.getFd()))
				channelsLeave.push_back(it->first);
		}

		for (size_t i = 0; i < channelsLeave.size(); ++i)
		{
			Channel* chan = server.getChannel(channelsLeave[i]);
			if (!chan)
				continue;

			std::string clientPrefix = client.getPrefix();
			const std::map<int, Client*>& members = chan->getMembers();
			for (std::map<int, Client*>::const_iterator itMem = members.begin(); itMem != members.end(); ++itMem)
			{
				server.sendMessage(itMem->first, MessageBuilder("PART")
					.setPrefix(clientPrefix)
					.setParam(channelsLeave[i]));
			}

			chan->removeMember(client.getFd());
			chan->removeOperator(client.getFd());

			if (chan->getMembers().empty())
				server.removeChannel(channelsLeave[i]);
		}
		return;
	}

	std::vector<std::string> channels;
	std::vector<std::string> keys;

	std::stringstream ssChan(prompt.args[0]);
	std::string tokenChan;
	while (std::getline(ssChan, tokenChan, ','))
	{
		channels.push_back(tokenChan);
	}

	if (prompt.args.size() > 1)
	{
		std::stringstream ssKeys(prompt.args[1]);
		std::string tokenKey;
		while (std::getline(ssKeys, tokenKey, ','))
			keys.push_back(tokenKey);
	}

	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string channelName = channels[i];
		std::string key = "";

		if (i < keys.size())
			key = keys[i];
		if (channelName.size() < 2|| channelName.size() > 50 || (channelName[0] != '#'))
		{
			server.sendMessage(client.getFd(), MessageBuilder("476")
				.setPrefix("ircserv")
				.setParam(client.getNickname())
				.setParam(channelName)
				.setContent("Bad Channel Mask"));
			continue ;
		}

		Channel* chan = server.getChannel(channelName);
		if (chan == NULL)
		{
			server.addChannel(channelName);
			chan = server.getChannel(channelName);
			chan->addOperator(client.getFd());
		}
		else
		{
			if (chan->hasMember(client.getFd()))
				continue ;
			if (chan->getInviteOnly() && !chan->isInvited(client.getNickname()))
			{
				server.sendMessage(client.getFd(), MessageBuilder("473")
					.setPrefix("ircserv")
					.setParam(client.getNickname())
					.setParam(channelName)
					.setContent("Cannot join channel (+i)"));
				continue ;
			}
			if (chan->hasKey() && chan->getKey() != key)
			{
				server.sendMessage(client.getFd(), MessageBuilder("475")
					.setPrefix("ircserv")
					.setParam(client.getNickname())
					.setParam(channelName)
					.setContent("Cannot join channel (+k)"));
				continue ;
			}
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
		chan->addMember(&client);
		chan->removeInvite(client.getNickname());
		std::string userPrefix = client.getPrefix();
		const std::map<int, Client*>& members = chan->getMembers();
		for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			server.sendMessage(it->first, MessageBuilder("JOIN")
				.setPrefix(userPrefix)
				.setParam(channelName));
		}
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
		std::string userList = "";
		for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			if (chan->isOperator(it->second->getFd()))
				userList += "@";
			userList += it->second->getNickname() + " ";
		}
		if (!userList.empty())
			userList.erase(userList.size() - 1);
		server.sendMessage(client.getFd(), MessageBuilder("353")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam("=")
			.setParam(channelName)
			.setContent(userList));
		server.sendMessage(client.getFd(), MessageBuilder("366")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(channelName)
			.setContent("End of /NAMES list"));
	}
}