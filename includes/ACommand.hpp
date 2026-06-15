#ifndef ACOMMAND_HPP
#define ACOMMAND_HPP

#include <string>
#include <vector>
#include "Parser.hpp"

class Server;
class Client;

class ACommand {
	public:
		virtual ~ACommand() {}
		
		virtual void execute(Server& server, Client& client, const IRCPrompt& prompt) = 0;

};

#endif