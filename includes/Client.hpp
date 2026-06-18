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
		std::string getOutBuffer() const;

		void	setNickname(const std::string &Nickname);
		void	setUsername(const std::string &Username);
		void	setIsRegistered(bool status);
		void	setHasPassword(bool status);
		void	setIp(const std::string &ip);

		void	appendBuffer(const std::string &str);
		void	appendOutBuffer(const std::string &str);
		void	clearBuffer();
		void	eraseOutBuffer(size_t len);

	private:
		int	_fd;
		std::string	_nickname;
		std::string	_username;
		bool	_isRegistered;
		std::string	_buffer;
		bool	_hasPassword;
		std::string	_ip;
		std::string _outBuffer;
};

#endif