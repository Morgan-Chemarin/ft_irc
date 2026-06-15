#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
	public:
		Client();
		Client(int fd);
		Client(const Client &src);
		Client &operator=(const Client &src);
		~Client();

		int	getFd() const;
		std::string	getNickname() const;
		std::string	getUsername() const;
		bool	getIsRegistered() const;
		std::string	getBuffer() const;
		bool	getHasPassword() const;
		std::string	getPrefix() const;

		void	setNickname(const std::string &Nickname);
		void	setUsername(const std::string &Username);
		void	setIsRegistered(bool status);
		void	setHasPassword(bool status);
		void	setIp(const std::string &ip);

		void	appendBuffer(const std::string &str);
		void	clearBuffer();

	private:
		int	_fd;
		std::string	_nickname;
		std::string	_username;
		bool	_isRegistered;
		std::string	_buffer; // sert a stocker les messages partiels
		bool	_hasPassword;
		std::string	_ip;
};

#endif