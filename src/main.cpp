#include "../includes/server.hpp"

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	std::string	password = av[2];
	int	port = std::atoi(av[1]); // Il faudra surement faire du parsing sur le port avant de l'init
	server	serv(port, password);
}
