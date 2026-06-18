#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <map>
#include <arpa/inet.h>
#include <csignal>

#include "Client.hpp"
#include "Parser.hpp"
#include "Channel.hpp"
#include "MessageBuilder.hpp"
#include "ACommand.hpp"
#include "CommandJoin.hpp"
#include "CommandPass.hpp"
#include "CommandNick.hpp"
#include "CommandUser.hpp"
#include "CommandMode.hpp"
#include "CommandPrivmsg.hpp"
#include "CommandKick.hpp"
#include "CommandTopic.hpp"
#include "CommandPing.hpp"
#include "CommandInvite.hpp"
#include "CommandPart.hpp"

extern volatile sig_atomic_t g_serverRunning;

class ACommand;

class Server
{
	public:
		Server();
		Server(int port, std::string password);
		Server(const Server &src);
		Server &operator=(const Server &src);
		~Server();

		void	initServer();
		void	run();

		Channel* getChannel(std::string const &name);
    	void	addChannel(std::string const &name);
		const std::map<std::string, Channel>& getChannels() const;

		std::map<int, Client>& getClients();
		Client* getClientByNickname(const std::string& nickname);

		std::string getPassword() const;

		void	checkRegistration(Client &client);
		void	sendMessage(int fd, const MessageBuilder &builder);

		void	removeChannel(const std::string &name);

		void	setPollOut(int fd,	bool enable);
		class  ErrorSocket : public std::exception
		{
			public:
				virtual const char *what() const throw()
				{
					return ("Error: Unable to create the server socket.");
				}
		};
		class  ErrorSocketOption : public std::exception
		{
			public:
				virtual const char *what() const throw()
				{
					return ("Error: Unable to configure socket options.");
				}
		};
		class  ErrorNonBlocking : public std::exception
		{
			public:
				virtual const char *what() const throw()
				{
					return ("Error: It is not possible to switch the socket to non-blocking mode.");
				}
		};
		class  ErrorBind : public std::exception
		{
			public:
				virtual const char *what() const throw()
				{
					return ("Error: Unable to bind the socket to the port.");
				}
		};
		class  ErrorSocketListen : public std::exception
		{
			public:
				virtual const char *what() const throw()
				{
					return ("Error: Unable to put the socket into listening mode.");
				}
		};

	private:
		int _port;
		std::string	_password;
		int	_serverSocket;
		std::vector<struct pollfd> _pollfd;
		std::map<int, Client>	_clients;
		std::map<std::string, Channel> _channels;
		std::map<std::string, ACommand*> _commands;

		void	acceptNewClient();
		void	receiveClientData(size_t i);
		void	disconnectClient(size_t i);

		void 	initCommands();

		void	processCLientCommand(int fd, std::string raw_line);
};

#endif