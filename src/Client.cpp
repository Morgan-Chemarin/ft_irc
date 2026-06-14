#include "Client.hpp"

Client::Client(): _fd(-1), _nickname(""), _username(""), _isRegistered(false), _buffer("")
{}

Client::Client(int fd): _fd(fd), _nickname(""), _username(""), _isRegistered(false), _buffer("")
{}

Client::Client(const Client &src)
{
	*this = src;
}

Client	&Client::operator=(const Client &src)
{
	if (this != &src)
	{
		_fd = src._fd;
		_nickname = src._nickname;
		_username = src._username;
		_isRegistered = src._isRegistered;
		_buffer = src._buffer;
	}
	return (*this);
}

Client::~Client()
{}

int	Client::getFd() const
{
	return (_fd);
}

std::string Client::getNickname() const
{
	return (_nickname);
}

std::string Client::getUsername() const
{
	return (_username);
}

bool Client::getIsRegistered() const
{
	return (_isRegistered);
}

std::string Client::getBuffer() const
{
	return (_buffer);
}

void	Client::setNickname(const std::string &nickname)
{
	_nickname = nickname;
}

void	Client::setUsername(const std::string &username)
{
	_username = username;
}

void	Client::setIsRegistered(bool status)
{
	_isRegistered = status;
}

void	Client::appendBuffer(const std::string &str)
{
	_buffer += str;
}

void	Client::clearBuffer()
{
	_buffer.clear();
}