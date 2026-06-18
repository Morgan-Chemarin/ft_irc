#include "Server.hpp"

volatile sig_atomic_t g_serverRunning = 1;

void handleSignal(int signum)
{
	(void)signum;
	g_serverRunning = false;
}

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
	}
	return (*this);
}

Server::~Server()
{
	if (_serverSocket != -1)
		close(_serverSocket);

	std::map<int, Client>::iterator itClient;
	for (itClient = _clients.begin(); itClient != _clients.end(); ++itClient)
	{
		if (itClient->first != -1)
			close(itClient->first);
	}
	_clients.clear();
	std::map<std::string, ACommand*>::iterator itCommand;
	for (itCommand = _commands.begin(); itCommand != _commands.end(); ++itCommand) {
		delete itCommand->second;
	}
	_commands.clear();
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
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1)
		throw ErrorSocket();
	int	optval = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
		throw ErrorSocketOption();
	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) == -1)
		throw ErrorNonBlocking();
	struct sockaddr_in serverAdress;
	std::memset(&serverAdress, 0, sizeof(serverAdress));
	serverAdress.sin_family = AF_INET;
	serverAdress.sin_addr.s_addr = INADDR_ANY;
	serverAdress.sin_port = htons(_port);
	if (bind(_serverSocket, (struct sockaddr *)&serverAdress, sizeof(serverAdress)) == -1)
		throw ErrorBind();
	if (listen(_serverSocket, SOMAXCONN) == -1)
		throw ErrorSocketListen();
}

void	Server::acceptNewClient()
{
	struct sockaddr_in	clientAdress;
	socklen_t	clientlen = sizeof(clientAdress);

	int	ClientFd = accept(_serverSocket, (struct sockaddr *)&clientAdress, &clientlen);
	if (ClientFd == -1)
	{
		std::cerr << "Error: function accept failed." << std::endl;
		return ;
	}
	fcntl(ClientFd, F_SETFL, O_NONBLOCK);
	_clients[ClientFd] = Client(ClientFd);
	std::string ipClient = inet_ntoa(clientAdress.sin_addr);
	_clients[ClientFd].setIp(ipClient);
	struct pollfd ClientPollFd;
	ClientPollFd.fd = ClientFd;
	ClientPollFd.events = POLLIN;
	ClientPollFd.revents = 0;
	_pollfd.push_back(ClientPollFd);
}

void	Server::disconnectClient(size_t i)
{
	int	fd = _pollfd[i].fd;
	close(fd);

	std::map<std::string, Channel>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		if (it->second.hasMember(fd))
			it->second.removeMember(fd);
		
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

void Server::processCLientCommand(int fd, std::string raw_line)
{
	IRCPrompt prompt = Parser::parsePrompt(raw_line);
	
	if (prompt.command == "CAP")
	{
		if (!prompt.args.empty() && prompt.args[0] == "LS")
		{
			sendMessage(fd, MessageBuilder("CAP")
				.setPrefix("ircserv")
				.setParam("*")
				.setParam("LS")
				.setContent(""));
		}
		return ;
	}

	if (!_clients[fd].getIsRegistered())
	{
		if (prompt.command != "PASS" && prompt.command != "NICK" && prompt.command != "USER" && prompt.command != "CAP")
		{
			sendMessage(fd, MessageBuilder("451")
				.setPrefix("ircserv")
				.setParam("*")
				.setContent("You have not registered"));
			return; 
		}
	}

	std::map<std::string, ACommand*>::iterator it = _commands.find(prompt.command);
	if (it == _commands.end())
	{
		std::string currentNick = _clients[fd].getNickname().empty() ? "*" : _clients[fd].getNickname();
		
		sendMessage(fd, MessageBuilder("421")
			.setPrefix("ircserv")
			.setParam(currentNick)
			.setParam(prompt.command)
			.setContent("Unknown command"));
		return;
	}
	
	it->second->execute(*this, _clients[fd], prompt);
}


void	Server::receiveClientData(size_t i)
{
	int	fd = _pollfd[i].fd;
	char buffer[1024];
	std::memset(buffer, 0, sizeof(buffer));
	int	bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (bytesRead <= 0)
		disconnectClient(i);
	else
	{
		_clients[fd].appendBuffer(buffer);
		std::string	current = _clients[fd].getBuffer();
		if (current.length() > 512 && current.find('\n') == std::string::npos)
		{
			disconnectClient(i);
			return ;
		}
		size_t	pos;
		while ((pos = current.find("\n")) != std::string::npos)
		{
			std::string command = current.substr(0, pos);
			if (!command.empty() && command[command.length() - 1] == '\r')
				command.erase(command.length() - 1);
			if (!command.empty())
				processCLientCommand(fd, command);
			current.erase(0, pos + 1);
		}
		_clients[fd].clearBuffer();
		_clients[fd].appendBuffer(current);
	}
}


void	Server::run()
{
	struct pollfd	serverPoll;
	serverPoll.fd = _serverSocket;
	serverPoll.events = POLLIN;
	serverPoll.revents = 0;
	_pollfd.push_back(serverPoll);

	while (g_serverRunning)
	{
		if (poll(&_pollfd[0], _pollfd.size(), -1) == -1)
		{
			if (!g_serverRunning)
				break;
			std::cerr << "Error: function poll failed" << std::endl;
			return ;
		}
		for (size_t i = 0; i < _pollfd.size(); i++)
		{
			if (_pollfd[i].revents & POLLIN)
			{
				if (_pollfd[i].fd == _serverSocket)
					acceptNewClient();
				else
				{
					size_t	oldSize = _pollfd.size();
					receiveClientData(i);
					if (_pollfd.size() < oldSize)
					{
						i--;
						continue;
					}
				}
			}
			if (_pollfd[i].revents & POLLOUT)
			{
				int currentFd = _pollfd[i].fd;
				std::string outData = _clients[currentFd].getOutBuffer();
				int bytesSent = send(currentFd, outData.c_str(), outData.length(), 0);
				if (bytesSent > 0)
				{
					_clients[currentFd].eraseOutBuffer(bytesSent);
					if (_clients[currentFd].getOutBuffer().empty())
						setPollOut(currentFd, false);	
				}
			}
		}
	}
}

void	Server::sendMessage(int fd, const MessageBuilder &builder)
{
	std::string	packet = builder.build();
	_clients[fd].appendOutBuffer(packet);
	setPollOut(fd, true);
}

void Server::checkRegistration(Client &client)
{
	if (client.getHasPassword() && !client.getNickname().empty() && !client.getUsername().empty() && !client.getIsRegistered())
	{
		client.setIsRegistered(true);
		sendMessage(client.getFd(), MessageBuilder("001")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setContent("Welcome to the Internet Relay Network " + client.getNickname()));

		sendMessage(client.getFd(), MessageBuilder("002")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setContent("Your host is ircserv, running version 1.0"));

		sendMessage(client.getFd(), MessageBuilder("003")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setContent("This server was created 2026-06-17"));

		sendMessage(client.getFd(), MessageBuilder("004")
			.setPrefix("ircserv")
			.setParam(client.getNickname())
			.setParam("ircserv")
			.setParam("1.0")
			.setParam("o")
			.setParam("itkol"));

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

void	Server::setPollOut(int fd, bool enable)
{
	for (size_t i = 0; i < _pollfd.size() ; i++)
	{
		if (_pollfd[i].fd == fd)
		{
			if (enable)
				_pollfd[i].events |= POLLOUT;
			else
				_pollfd[i].events &= ~POLLOUT;
			break ;
		}
	}
}