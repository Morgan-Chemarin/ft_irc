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
		std::string	getTopic() const;
		void setTopic(const std::string &topic);
		
		void addMember(Client *client);
		void removeMember(int fd);
		bool hasMember(int fd) const;

		bool getInviteOnly() const;
		void setInviteOnly(bool);

		bool getTopicProtected() const;
		void setTopicProtected(bool);

		void setKey(std::string key);
		void unsetKey();
		bool hasKey() const;
		const std::string& getKey() const;

		void setLimitUsers(int limit);
		void unsetLimitUsers();
		bool hasLimit() const;
		int getLimitUsers() const;

		void addOperator(int fd);
		void removeOperator(int fd);

		bool isOperator(int fd) const;

		void addInvite(const std::string& nickname);
		void removeInvite(const std::string& nickname);
		bool isInvited(const std::string& nickname) const;

	private:
		std::string _name;
		std::string	_topic;

		bool _isInviteOnly;
		bool _isTopicProtected;
	
		std::string _key;
		int _limitUsers;
		std::map<int, Client*> _membersList;
		std::set<int> _operatorList;

		std::set<std::string> _invitedUsers;

};

#endif