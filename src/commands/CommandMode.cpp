#include "CommandMode.hpp"
#include "Server.hpp"

void CommandMode::execute(Server& server, Client& client, const IRCPrompt& prompt)
{
	(void)client;

	if (prompt.args.empty())
	{
		server.sendMessage(client.getFd(), MessageBuilder("461")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam("MODE")
			.setContent("Not enough parameters"));
		return ;
	}

	std::string chanName = prompt.args[0];
	Channel* chan = server.getChannel(chanName);
	if (!chan)
	{
		server.sendMessage(client.getFd(), MessageBuilder("403") 
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(chanName)
			.setContent("No such channel"));
		return;
	}

	if (!chan->hasMember(client.getFd()))
	{
		server.sendMessage(client.getFd(), MessageBuilder("442") 
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(chanName)
			.setContent("You're not on that channel"));
		return;
	}

	if (prompt.args.size() == 1)
	{
		std::string modes = "+";
		std::string modesParams = "";

        if (chan->getInviteOnly()) modes += "i";
        if (chan->getTopicProtected()) modes += "t";
        if (chan->hasKey())
		{
            modes += "k";
            modesParams += " " + chan->getKey();
        }
		if (chan->hasLimit())
		{
            modes += "l";
            std::stringstream ss;
            ss << chan->getLimitUsers();
            modesParams += " " + ss.str();
        }
		server.sendMessage(client.getFd(), MessageBuilder("324")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(chanName)
			.setParam(modes + modesParams));
		return ;
	}

	if (!chan->isOperator(client.getFd()))
	{
		server.sendMessage(client.getFd(), MessageBuilder("482")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(chanName)
			.setContent("You're not channel operator"));
		return;
	}

	bool sign = true;
	std::string modeString = prompt.args[1];
	size_t argIndex = 2;

	std::string modeChanges = "";
	std::vector<std::string> paramsChanges;
	char lastSign = '\0';

	for (size_t i = 0; i < modeString.size(); i++)
	{
		char c = modeString[i];

		if (c == '+')
		{
			sign = true;
			continue;
		}
		if (c == '-')
		{
			sign = false;
			continue;
		}

		char curSignChar = sign ? '+' : '-';
		bool changed = false;
		std::string paramAdded = "";

		switch (c)
		{
			case 'i':
				if (chan->getInviteOnly() != sign)
				{
					chan->setInviteOnly(sign);
					changed = true;
				}
				break;

			case 't':
				if (chan->getTopicProtected() != sign)
				{
					chan->setTopicProtected(sign);
					changed = true;
				}
				break;

			case 'k':
				if (sign == true)
				{
					if (argIndex < prompt.args.size())
					{
						std::string password = prompt.args[argIndex++];
						chan->setKey(password);
						changed = true;
						paramAdded = password;
					}
				}
				else
				{
					chan->unsetKey();
					changed = true;
				}
				break;

			case 'l':
				if (sign == true)
				{
					if (argIndex < prompt.args.size())
					{
						std::string limitStr = prompt.args[argIndex++];
                        bool isEmpty = !limitStr.empty();
                        for (size_t k = 0; k < limitStr.size(); ++k) {
                            if (limitStr[k] < '0' || limitStr[k] > '9')
                                isEmpty = false;
                        }
                        
                        if (isEmpty) {
                            int limit = std::atoi(limitStr.c_str());
                            if (limit > 0) {
                                chan->setLimitUsers(limit);
                                changed = true;
                                paramAdded = limitStr;
                            }
                        }
					}
				}
				else
				{
					chan->unsetLimitUsers();
					changed = true;
				}
				break;

			case 'o':
				if (argIndex < prompt.args.size())
				{
					std::string targetNick = prompt.args[argIndex++];
					Client* targetClient = server.getClientByNickname(targetNick);
					if (targetClient != NULL)
					{
						if (chan->hasMember(targetClient->getFd()))
						{
							if (sign == true && !chan->isOperator(targetClient->getFd()))
							{
								chan->addOperator(targetClient->getFd());
								changed = true;
								paramAdded = targetNick;
							}
							else if (sign == false && chan->isOperator(targetClient->getFd()))
							{
								chan->removeOperator(targetClient->getFd());
								changed = true;
								paramAdded = targetNick;
							}
						}
						else
						{
							server.sendMessage(client.getFd(), MessageBuilder("441")
								.setPrefix("ircserv")
								.setParam(client.getNickname())
								.setParam(targetNick)
								.setParam(chanName)
								.setContent("They aren't on that channel"));
						}
					}
					else
					{
						server.sendMessage(client.getFd(), MessageBuilder("401")
							.setPrefix("ircserv")
							.setParam(client.getNickname())
							.setParam(targetNick)
							.setContent("No such nick/channel"));
					}
				}
				break;
		}

		if (changed)
		{
			if (lastSign != curSignChar)
			{
				modeChanges += curSignChar;
				lastSign = curSignChar;
			}
			modeChanges += c;
			if (!paramAdded.empty())
				paramsChanges.push_back(paramAdded);
		}
	}

	if (!modeChanges.empty())
	{
		MessageBuilder msg("MODE");
		msg.setPrefix(client.getPrefix())
		   .setParam(chanName)
		   .setParam(modeChanges);

		for (size_t i = 0; i < paramsChanges.size(); ++i)
			msg.setParam(paramsChanges[i]);

		const std::map<int, Client*>& members = chan->getMembers();
		for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			server.sendMessage(it->first, msg);
		}
	}
}