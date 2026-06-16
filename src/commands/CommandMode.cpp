#include "CommandMode.hpp"
#include "Server.hpp"
// 
// MODE #salon +i
// MODE #salon +k secret123
// MODE #salon +it
// MODE #salon +kl secret123 50
// 
// 
// 

void CommandMode::execute(Server& server, Client& client, const IRCPrompt& prompt)
{
	(void)client;
	// 461 ERR_NEEDMOREPARAMS

	std::string chanName = prompt.args[0];
	Channel* chan = server.getChannel(chanName);
	if (!chan)
	{
		// 403 ERR_NOSUCHCHANNEL si le canal existe pas
		return;
	}

	// 442 ERR_NOTONCHANNEL si le client nes tpas sur le channel

	// 482 ERR_CHANOPRIVSNEEDED si le client nest pas operator sur le channel

	bool sign = true;
	std::string modeString = prompt.args[1];
	size_t argIndex = 2;

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

		switch (c)
		{
			case 'i': // toggle inviteonly/topicprotected
				chan->setInviteOnly(sign);
				break;

			case 't':
				chan->setTopicProtected(sign);
				break;

			case 'k':
				if (sign == true)
				{
					if (argIndex < prompt.args.size())
					{
						std::string password = prompt.args[argIndex++];
						chan->setKey(password);
					}
				}
				else
					chan->unsetKey();
				break;

			case 'l':
				if (sign == true)
				{
					if (argIndex < prompt.args.size())
					{
						int limit = std::atoi(prompt.args[argIndex++].c_str());
						chan->setLimitUsers(limit);
					}
				}
				else 
					chan->unsetLimitUsers();
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
							if (sign == true)
								chan->addOperator(targetClient->getFd());
							else
								chan->removeOperator(targetClient->getFd());
						}
						else
						{
							// 441 ERR_USERNOTINCHANNEL
						}
					}
					else
					{
						// 401 ERR_NOSUCHNICK
					}
				}
				break;
			
			// default:
				// break;
		}
	}

	// broadcast des changements 

}