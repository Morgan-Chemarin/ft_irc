#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <set>
#include <sys/socket.h>

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
		
		void addMember(Client *client);
        void removeMember(int fd);
        bool hasMember(int fd) const;

		bool getInviteOnly() const;
		void setInviteOnly(bool);

		bool getTopicProtected() const;
		void setTopicProtected(bool);

		void setKey(std::string key);
		void unsetKey();

		void setLimitUsers(int limit);
		void unsetLimitUsers();

		void addOperator(int fd);
		void removeOperator(int fd);

		bool isOperator(int fd) const;

	private:
		std::string _name;
		std::map<int, Client*> _membersList;
		
		// option mode
		bool _isInviteOnly;
		bool _isTopicProtected;
		std::string _key;
		int _limitUsers;
		std::set<int> _operatorList;

};

#endif