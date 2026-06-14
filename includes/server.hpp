#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

class Server
{
	public:
		Server();
		Server(int port, std::string password);
		Server(const Server &src);
		Server &operator=(const Server &src);
		~Server();

		void	initServer();

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
};

#endif