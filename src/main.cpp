#include "Server.hpp"
#include "Channel.hpp"

int main(int ac, char **av)
{
	Client bob(3);
	Channel room1("room1");
	room1.addMember(&bob);
	std::cout << room1.hasMember(3);

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
		serv.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
