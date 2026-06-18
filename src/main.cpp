#include "Server.hpp"

void handleSignal(int signum);

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	std::string portStr(av[1]);
	if (portStr.empty())
	{
		std::cerr << "Error: Port cannot be empty." << std::endl;
		return 1;
	}

	for (size_t i = 0; i < portStr.size(); ++i)
	{
		if (!std::isdigit(portStr[i]))
		{
			std::cerr << "Error: Invalid port. Must contain digits only." << std::endl;
			return 1;
		}
	}

	long portCheck = std::atol(av[1]);
	if (portCheck < 1024 || portCheck > 65535)
	{
		std::cerr << "Error: Invalid port. Range must be [1024 - 65535]." << std::endl;
		return 1;
	}
	int port = static_cast<int>(portCheck);

	std::string password(av[2]);
	if (password.empty())
	{
		std::cerr << "Error: Password cannot be empty." << std::endl;
		return 1;
	}

	for (size_t i = 0; i < password.size(); ++i)
	{
		if (std::isspace(password[i]) || std::iscntrl(password[i]))
		{
			std::cerr << "Error: Invalid password. Spaces or control characters are forbidden." << std::endl;
			return 1;
		}
	}

	std::signal(SIGINT, handleSignal);
	std::signal(SIGTERM, handleSignal);

	try
	{
		Server	serv(port, password);
		serv.initServer();
		serv.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Server crash: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
