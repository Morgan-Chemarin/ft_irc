#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>

class Channel
{
	public:
		Channel();
		Channel(const Channel& other);
		Channel& operator=(const Channel& other);
		~Channel();

		// add client to channel
		// remove client to channel
		// gerer mode
		// stocke TOPIC

	private:
		// std::map<int, Client*> clientsList;
		// std::map<int, Client*> operatorsList;

};

#endif