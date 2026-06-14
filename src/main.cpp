#include "../includes/server.hpp"

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	try
	{
		int	port = std::atoi(av[1]); // Il faudra surement faire du parsing sur le port avant de l'init
		Server	serv(port, av[2]);
		serv.initServer();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
