#include "../includes/server.hpp"

server::server()
{}

server::server(int port, std::string password)
{
	_port = port;
	_password = password;
}

server::server(const server &src)
{
	_port = src._port;
	_password = src._password;
}

server	&server::operator=(const server &src)
{
	if (this != &src)
	{
		_port = src._port;
		_password = src._password;
	}
	return (*this);
}

server::~server()
{}

void	server::initServer()
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
	setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}