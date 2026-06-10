#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include "Client.hpp"
#include "Channel.hpp"

class Server
{
	public:
		Server();
		Server(const Server& other);
		Server& operator=(const Server& other);
		~Server();

		Client* getClientByNick(std::string const &nick); // pour PRIVMSG skip
		Channel* getChannelByName(std::string const &name); // si un canal existe
		void addChannel(Channel* chan); // JOIN

		void broadcastToChannel(Channel const &chan, std::string const &msg, int excludeFd);
	private:
};


#endif