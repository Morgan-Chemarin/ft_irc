#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
	public:
		Client();
		Client(const Client& other);
		Client& operator=(const Client& other);
		~Client();

		std::string getNickName() const;
		std::string getUsername() const;
		int getFd() const;
		void appendToReadBuffer(std::string const& str);
		std::string getNextCommand();
		void sendToClient(std::string const &msg);

	private:
		std::string nickname;
		std::string username;
		int fd;
};

#endif