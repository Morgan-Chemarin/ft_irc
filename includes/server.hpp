#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

class server
{
	public:
		server();
		server(int port, std::string password);
		server(const server &src);
		server &operator=(const server &src);
		~server();

		void	initServer();

		class  ErrorSocket : public std::exception
		{
			public:
				virtual const char *what() const throw()
				{
					return ("Error: Unable to create the server socket.");
				}
		};
	private:
		int _port;
		std::string	_password;
		int	_serverSocket;
};

#endif