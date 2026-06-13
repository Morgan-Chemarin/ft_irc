#include "../includes/server.hpp"

Server::Server()
{}

Server::Server(int port, std::string password)
{
	_port = port;
	_password = password;
}

Server::Server(const Server &src)
{
	_port = src._port;
	_password = src._password;
}

Server	&Server::operator=(const Server &src)
{
	if (this != &src)
	{
		_port = src._port;
		_password = src._password;
	}
	return (*this);
}

Server::~Server()
{}

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
}