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
#include <unistd.h>
#include <arpa/inet.h>

#include "Client.hpp"
#include "Parser.hpp"
#include "Channel.hpp"
#include "MessageBuilder.hpp"

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

		std::map<int, Client>& getClients();
		Client* getClientByNickname(const std::string& nickname);

		std::string getPassword() const;

		void	checkRegistration(Client &client);
		void	sendMessage(int fd, const MessageBuilder &builder);

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
		// _pollfd est le vector que va surveiller la fonction poll(), quand un message est envoye elle va recevoir
		// une alerte et directement aller chercher le fd correspondant
		// On utilise un vector car le nombre de client va souvent changer et donc le vector permet d'ajouter des
		// clients facilemet (push_back) et aussi car poll() a beosin d'un conteneur avec une memoire contigu (tout les elements se touchent)
		std::vector<struct pollfd> _pollfd; // struct pollfd : fd = numero de socket a cible, events = c'est la ou l'on va recevoir l'alerte du message, revents = c'est la que poll() ecrire sa reponse
		// Une fois le fd retrouve dans _pollfd, poll() va ensuite associe le client correspondant au fd qui a envoye l'alerte
		// On utlise une map car sans elle on devrait chercher dans tout le conteneur a qui appartient le fd trouver juste avant alors que 
		// grace a la clef (ici les fd) des map on peut directement savoir a qui il appartient
		std::map<int, Client>	_clients;

		//la liste des channels existant <nomduchannel, Instance du channel>
		std::map<std::string, Channel> _channels;

		// la liste des functions de command lie a leur nom, init dans le constructor
		std::map<std::string, ACommand*> _commands;

		void	acceptNewClient();
		void	receiveClientData(size_t i);
		void	disconnectClient(size_t i);

		// init la map de funcitons de commande
		void 	initCommands();

		void	processCLientCommand(int fd, std::string raw_line);
};

#endif