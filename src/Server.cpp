#include "Server.hpp"
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

Server::Server()
{}

Server::Server(int port, std::string password)
{
	_port = port;
	_password = password;
	_serverSocket = -1;

	initCommands();
}

Server::Server(const Server &src)
{
	_port = src._port;
	_password = src._password;
	_serverSocket = src._serverSocket;

	initCommands();
}

Server	&Server::operator=(const Server &src)
{
	if (this != &src)
	{
		_port = src._port;
		_password = src._password;
		_serverSocket = src._serverSocket;

		// pour la rigueur ptretre bien assigne les maps ou les del puis init
	}
	return (*this);
}

Server::~Server()
{
	if (_serverSocket != -1)
		close(_serverSocket);

	// pour vider la map de commande ala destrutcion
	std::map<std::string, ACommand*>::iterator it;
	for (it = _commands.begin(); it != _commands.end(); ++it) {
		delete it->second;
	}
}

void Server::initCommands() {
	_commands["JOIN"] = new CommandJoin();
    _commands["PASS"] = new CommandPass();
	_commands["TOPIC"] = new CommandTopic();
    _commands["NICK"] = new CommandNick();
    _commands["USER"] = new CommandUser();
	_commands["PRIVMSG"] = new CommandPrivmsg();
	_commands["KICK"] = new CommandKick();
	_commands["MODE"] = new CommandMode();
	_commands["PING"] = new CommandPing();
	_commands["INVITE"] = new CommandInvite();
	_commands["PART"] = new CommandPart();
}

void Server::addChannel(std::string const &name)
{
	_channels[name] = Channel(name);
}

Channel* Server::getChannel(std::string const &name)
{
	std::map<std::string, Channel>::iterator it = _channels.find(name);
	if (it == _channels.end())
		return NULL;
	return &(it->second);
}

const std::map<std::string, Channel>& Server::getChannels() const
{
    return _channels;
}

std::map<int, Client>& Server::getClients()
{
	return _clients;
}

Client* Server::getClientByNickname(const std::string& nickname)
{
	std::map<int, Client>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second.getNickname() == nickname)
			return &(it->second);
	}
	return NULL;
}

std::string Server::getPassword() const
{
	return _password;
}

void	Server::initServer()
{
	// init d'un socket: AF_INET = internet IPV4, SOCK_STREAM = Protocole TCP(Si un paquet est perdu il sera renvoyer),
	// 0 = numero de protocole mais dcp pas besoin on met 0
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1)
		throw ErrorSocket();
	// Permet de ne pas liberer un port lors d'un crash ou autre, il met le port en attente pedant quelques seconde afin de rendre la reconnexion possible
	// _serverSocket = cible, SOL_SOCKET = couche (modele OSI) a laquelle on veut apporter des modif,
	// SO_REUSEADDR = Nom de l'option que je veux modifer dans la couche SOL_SOCKET, optval = interupteur pour activer ou desactiver l'option (1 = activer & 0 = desactiver)
	int	optval = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
		throw ErrorSocketOption();
	// permet de passer le socket en mode non bloquant (trop chiant a expliquer a l'ecrit), _serverSocket = cible,
	// F_SETFL = permet de dire au socket que l'on s'apprete a modifier sa configuration, O_NONBLOCK = le nouveau 
	// comportement que l'on veut apporter a notre fonction
	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) == -1)
		throw ErrorNonBlocking();
	// preparation de l'adresse du server
	struct sockaddr_in serverAdress; // struct concu pour stocker les infornations de connexion reseau IPV4
	std::memset(&serverAdress, 0, sizeof(serverAdress)); // on remplit notre adresse de 0 pour la securite
	serverAdress.sin_family = AF_INET; // on dit au serveur d'utiliser le format IPV4
	serverAdress.sin_addr.s_addr = INADDR_ANY; // defini les adresses IP que notre serveur doit ecouter (toutes les adresses ici)
	serverAdress.sin_port = htons(_port); // defini le port du server, htons = reorganise le port pour qu'il soit lisible par le protocole internet
	// assosciation du socket et des informations de connexion 
	if (bind(_serverSocket, (struct sockaddr *)&serverAdress, sizeof(serverAdress)) == -1)
		throw ErrorBind();
	// permet de mettre le serveur en ecoute sur le port indique avant, _serverSocket = cible, SOMAXCONN = si jamais il y a beaucoup de
	// requetes envoyer sur le port, le serveur va creer une file d'attente, cette option est la pour indique de creer la plus grande file possible
	if (listen(_serverSocket, SOMAXCONN) == -1)
		throw ErrorSocketListen();
}

// Cette Fonction est appelee des que poll() detecte de l'activite sur _serverSocket, la fonction va creer un
// fd uniquement pour la communication entre le client et le serveur. Elle va aussi configurer le nouveau client
// en non bloquant puis elle va associer une struct pollfd a notre client.

void	Server::acceptNewClient()
{
	struct sockaddr_in	clientAdress; // va permettre de retrouver l'adresse IP et le port de notre client
	socklen_t	clientlen = sizeof(clientAdress);

	int	ClientFd = accept(_serverSocket, (struct sockaddr *)&clientAdress, &clientlen); // creation du nouveau fd entre le client et le serveur
	if (ClientFd == -1)
	{
		std::cerr << "Error: function accept failed." << std::endl;
		return ;
	}
	fcntl(ClientFd, F_SETFL, O_NONBLOCK); // non bloquant
	_clients[ClientFd] = Client(ClientFd);
	std::string ipClient = inet_ntoa(clientAdress.sin_addr); // inet_ntoa permet de convertir l'IP en binaire en string
	_clients[ClientFd].setIp(ipClient);
	struct pollfd ClientPollFd;
	ClientPollFd.fd = ClientFd;
	ClientPollFd.events = POLLIN;
	ClientPollFd.revents = 0;
	_pollfd.push_back(ClientPollFd);
	std::cout << "Client connected" << std::endl;
}

// Cette fontion est appelee lorsque un client quitte son terminal ou bien crash,
// elle va faire en sorte de supprime tout ce que l'on a sur le client concerne.

void	Server::disconnectClient(size_t i)
{
	int	fd = _pollfd[i].fd;
	std::cout << "Client disconnected" << std::endl;
	close(fd);

	// gerer lexpulsion des channels ou il etait present
	std::map<std::string, Channel>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		// sil est dans le channel on le vire
		if (it->second.hasMember(fd))
			it->second.removeMember(fd);
		
		// sil ny a plus personne dans channel on le supprime
		if (it->second.getMembers().empty())
		{
			std::map<std::string, Channel>::iterator to_delete = it;
			++it;
			_channels.erase(to_delete);
		}
		else
			++it;
	}

	_clients.erase(fd);
	_pollfd.erase(_pollfd.begin() + i);
}

// on parse le prompt dans une struct et on cherche dans la map e command on on a une correspondance
// si il y en a une on execute la fonction execute du 2eme element (Command *)
void Server::processCLientCommand(int fd, std::string raw_line)
{
	IRCPrompt prompt = Parser::parsePrompt(raw_line);
	
	if (prompt.command == "CAP")
	{
		return;  //! enlever le warning de cap sans auth
	}

	std::map<std::string, ACommand*>::iterator it = _commands.find(prompt.command);

	// si la commande existe pas
	if (it == _commands.end())
	{
		std::cout << "Command " << prompt.command << " doesn't exist." << std::endl;
		return;
	}
	
	// si on est pas enregistre  on ne peut que faire USER NICK ET PASS
	if (!_clients[fd].getIsRegistered())
	{
		if (prompt.command != "PASS" && prompt.command != "NICK" && prompt.command != "USER")
		{
			std::cout << "Fd " << fd << " execute  " << prompt.command << " without authentication !" << std::endl;
			return; 
		}
	}
	
	it->second->execute(*this, _clients[fd], prompt);
}

// Cette fonction gere la lecture des donnee. Son role est de prevenir la fonction disconnectClient
// si il y a une deco, mais surtout a servir de relais entre le client et processCommand()
// 

void	Server::receiveClientData(size_t i)
{
	int	fd = _pollfd[i].fd;
	char buffer[1024]; // 1024 car 1024 octets = 1 Kio et donc le processeur peut aligner parfaitement cette structure sur les
					   // blocs de memoire physique de la machine et aussi cela assure de quasiment pouvoir vider un paquet en un seul coup
	std::memset(buffer, 0, sizeof(buffer));
	int	bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0); // on lit ce que le client a envoye, le resultat sera stocke dans buffer
	if (bytesRead <= 0) // Si le client renvoie 0 ou -1 cela veut dire que le client a fermer la connexion
		disconnectClient(i);
	else
	{
		_clients[fd].appendBuffer(buffer);
		std::string	current = _clients[fd].getBuffer();
		size_t	pos;
		// on creer une boucle pour pouvoir traiter plusieurs commandes envoye en un
		// seul paquet, chaque commande est separe d'un \n
		while ((pos = current.find("\n")) != std::string::npos)
		{
			std::string command = current.substr(0, pos); // on enleve le \n de la commande
			// on enleve le \r si il existe
			if (!command.empty() && command[command.length() - 1] == '\r')
				command.erase(command.length() - 1);
			if (!command.empty())
				processCLientCommand(fd, command);
			current.erase(0, pos + 1);
		}
		// si jamais la commande est recu en deux paquet alors on sotck la premiere partie dans le buffer
		// du client pour qu'a la reception du prochain paquet on ai la commande entiere
		_clients[fd].clearBuffer();
		_clients[fd].appendBuffer(current);
	}
}

// le role de la fonction run est d'etre alerte si le serveur recois un paquet
// et ensuite de renvoyer ce paquet vers les fonctions : acceptNewClient et ReceiveClientData

void	Server::run()
{
	struct pollfd	serverPoll; // creation du serverPoll pour _serverSocket
	serverPoll.fd = _serverSocket;
	serverPoll.events = POLLIN;
	serverPoll.revents = 0;
	_pollfd.push_back(serverPoll);

	while (true)
	{
		if (poll(&_pollfd[0], _pollfd.size(), -1) == -1) // Permet de mettre le programme en pause jusqu'a l'arriver d'un paquet (Reduit la conso du CPU)
		{
			std::cerr << "Error: function poll failed" << std::endl;
			return ;
		}
		for (size_t i = 0; i < _pollfd.size(); i++) // on cherche dans _pollfd de qu'elle client provient le paquet
		{
			if (_pollfd[i].revents & POLLIN) // On regarde si le fd en question a recu le FLAG POLLIN, si oui c'est ce fd qui a envoye un paquet
			{
				if (_pollfd[i].fd == _serverSocket) // _serverSocket est forcement egal a nouveau client
					acceptNewClient();
				else
				{
					size_t	oldSize = _pollfd.size();
					receiveClientData(i);
					if (_pollfd.size() < oldSize) // Si jamais il y a une deconnexion d'un client, le client d'apres dans _pollfd va prendre sa
						i--;					  // place et donc pour eviter de sauter son tour (i++) ou prevois le coup en le decrementant
				}
			}
		}
	}
}

// Cette fonction permet d'envoyer un message au client via le socket

void	Server::sendMessage(int fd, const MessageBuilder &builder)
{
	std::string	packet = builder.build();
	std::cout << packet << std::endl;
	send(fd, packet.c_str(), packet.length(), 0);
}

// Cette fonction permet de check si les 3 conditions (le client a un password, un nickname, un username) sont remplis.
// Si oui elle va mettre le bool _isRegistered a true et donc le client pourra executer d'autres commandes

void Server::checkRegistration(Client &client)
{
	if (client.getHasPassword() && !client.getNickname().empty() && !client.getUsername().empty() && !client.getIsRegistered())
	{
		client.setIsRegistered(true);

		// 001 RPL_WELCOME visiblement quand on sinscrit on doit envoyer des messages de bienvneue sinon le client est pas content
		sendMessage(client.getFd(), MessageBuilder("001")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setContent("Welcome to the Internet Relay Network " + client.getNickname()));

		// 002 RPL_YOURHOST cest la version du server et son nom
		sendMessage(client.getFd(), MessageBuilder("002")
            .setPrefix("ircserv")
            .setParam(client.getNickname())
            .setContent("Your host is ircserv, running version 1.0"));
		
		// 003 RPL_CREATED pour savoir quand le serveur a ete cree
		sendMessage(client.getFd(), MessageBuilder("003")
            .setPrefix("ircserv")
            .setParam(client.getNickname())
            .setContent("This server was created 2026-06-17"));

		// 004 RPL_MYINFO cest les infos technique , donc le nom du server, sa version, les modes dutilisateurs (operator), les options de channel supportees
		sendMessage(client.getFd(), MessageBuilder("004")
            .setPrefix("ircserv")
            .setParam(client.getNickname())
            .setParam("ircserv")
            .setParam("1.0")
            .setParam("o")
            .setParam("itkol"));

		// 422 ERR_NOMOTD MessageOfTheDay mais on a pas ce fichier
		sendMessage(client.getFd(), MessageBuilder("422")
            .setPrefix("ircserv")
            .setParam(client.getNickname())
            .setContent("MOTD File is missing"));
	}
}

void	Server::removeChannel(const std::string &name)
{
	_channels.erase(name);
}