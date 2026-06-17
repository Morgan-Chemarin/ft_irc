#include "CommandMode.hpp"
#include "Server.hpp"

void CommandMode::execute(Server& server, Client& client, const IRCPrompt& prompt)
{
	(void)client;

	if (prompt.args.empty())
	{
		// 461 ERR_NEEDMOREPARAMS : pas assez de paramettre mais ne sera jamais appelle sur hexchat (il comprend quon parle du channel actuelle)
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
		// 403 ERR_NOSUCHCHANNEL : si le channel nexiste pas (hexchat skip et gere MODE room sans #)
		server.sendMessage(client.getFd(), MessageBuilder("403") 
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(chanName)
			.setContent("No such channel"));
		return;
	}

	if (!chan->hasMember(client.getFd()))
	{
		// 442 ERR_NOTONCHANNEL : si le client nest pas sur le channel cible
		server.sendMessage(client.getFd(), MessageBuilder("442") 
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(chanName)
			.setContent("You're not on that channel"));
		return;
	}

	if (prompt.args.size() == 1)
	{
		// pour avoir toutes les options actives et leur value associe en deux string
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

		// 324 RPL_CHANNELMODEIS : renvoi la liste des modes de #room quand on fait "MODE #room"
		server.sendMessage(client.getFd(), MessageBuilder("324")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam(chanName)
			.setParam(modes + modesParams));
		return ;
	}

	if (!chan->isOperator(client.getFd()))
	{
		// 482 ERR_CHANOPRIVSNEEDED : si le client nest pas operator sur le channel
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
			case 'i': // toggle inviteonly
				if (chan->getInviteOnly() != sign)
				{
					chan->setInviteOnly(sign);
					changed = true;
				}
				break;

			case 't': // toggle topicProtected
				if (chan->getTopicProtected() != sign)
				{
					chan->setTopicProtected(sign);
					changed = true;
				}
				break;

			case 'k': // password
				if (sign == true) // add key
				{
					if (argIndex < prompt.args.size())
					{
						std::string password = prompt.args[argIndex++];
						chan->setKey(password);
						changed = true;
						paramAdded = password;
					}
				}
				else // remove key
				{
					chan->unsetKey();
					changed = true;
				}
				break;

			case 'l': // limitUsers
				if (sign == true) // add limit ( > 0 )
				{
					if (argIndex < prompt.args.size())
					{
						std::string limitStr = prompt.args[argIndex++];
						int limit = std::atoi(limitStr.c_str());
						chan->setLimitUsers(limit);
						changed = true;
						paramAdded = limitStr;
					}
				}
				else // remove limit ( 0 )
				{
					chan->unsetLimitUsers();
					changed = true;
				}
				break;

			case 'o': // set Operator
				if (argIndex < prompt.args.size())
				{
					std::string targetNick = prompt.args[argIndex++];
					Client* targetClient = server.getClientByNickname(targetNick);
					if (targetClient != NULL)
					{
						if (chan->hasMember(targetClient->getFd())) // si la cible est dans le channel
						{
							if (sign == true && !chan->isOperator(targetClient->getFd())) // soit si cest + et que la cible nest pas operator on met
							{
								chan->addOperator(targetClient->getFd());
								changed = true;
								paramAdded = targetNick;
							}
							else if (sign == false && chan->isOperator(targetClient->getFd())) // sinon sil les on lui enleve
							{
								chan->removeOperator(targetClient->getFd());
								changed = true;
								paramAdded = targetNick;
							}
						}
						else
						{
							// 441 ERR_USERNOTINCHANNEL : quand la cible est sur le serveur mais pas sur le channel
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
						// 401 ERR_NOSUCHNICK : on ne trouve pas le client sur le server
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
			if (lastSign != curSignChar) // on ajoute le signe que si cest pas le meme que le precedent
			{
				modeChanges += curSignChar;
				lastSign = curSignChar;
			}
			modeChanges += c;
			if (!paramAdded.empty())
				paramsChanges.push_back(paramAdded);
		}
	}

	// broadcast des changements 
	if (!modeChanges.empty())
	{
		// on cree le message de broaadcast
		MessageBuilder msg("MODE");
		msg.setPrefix(client.getPrefix())
		   .setParam(chanName) // #room1
		   .setParam(modeChanges); // -oi

		for (size_t i = 0; i < paramsChanges.size(); ++i) // Momo
			msg.setParam(paramsChanges[i]);
		// Name!user@host MODE #room1 +oi Momo

		// on envoi a tous les fd du channel
		const std::map<int, Client*>& members = chan->getMembers();
		for (std::map<int, Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			server.sendMessage(it->first, msg);
		}
	}
}