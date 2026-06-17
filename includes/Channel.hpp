#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>

#include "Client.hpp"

class Channel
{
	public:
		Channel();
		Channel(const std::string& name);
		Channel(const Channel& other);
		Channel& operator=(const Channel& other);
		~Channel();

		std::string getName() const;
        const std::map<int, Client*> &getMembers() const;
		std::string	getTopic() const;
		void setTopic(const std::string &topic);
		
		void addMember(Client *client);
        void removeMember(int fd);
        bool hasMember(int fd) const;
		
	private:
		std::string _name;
		std::map<int, Client*> _membersList;
		// std::map<int, Client*> operatorsList;
		std::string	_topic;
};

#endif