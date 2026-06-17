#include "Channel.hpp"

Channel::Channel(): 
	_name(""), 
    _isInviteOnly(false), 
    _isTopicProtected(false),
    _key(""), 
    _limitUsers(0)
{

}

Channel::Channel(const std::string& name): 
	_name(name), 
    _isInviteOnly(false), 
    _isTopicProtected(false),
    _key(""), 
    _limitUsers(0)
{

}

Channel::Channel(const Channel& other)
{
	_name = other._name;
	_membersList = other._membersList;
	_topic = other._topic;
}

Channel& Channel::operator=(const Channel& other)
{
	if (this != &other)
	{
		_name = other._name;
		_membersList = other._membersList;
		_topic = other._topic;
	}
	return *this;
}

Channel::~Channel()
{

}

std::string Channel::getName() const
{
	return _name;
}

// pour recuperer tous les membres du channel
const std::map<int, Client*> &Channel::getMembers() const
{
	return _membersList;
}

std::string Channel::getTopic() const
{
	return _topic;
}

void	Channel::setTopic(const std::string &topic)
{
	_topic = topic;
}

// pour ajouter un membre au channel
void Channel::addMember(Client *client)
{
    if (client)
        _membersList[client->getFd()] = client;
}

// pour supprimer un mec du channel
void Channel::removeMember(int fd)
{
    _membersList.erase(fd);
}

// est ce que le user fd est dans le channel ?
bool Channel::hasMember(int fd) const
{
    return (_membersList.find(fd) != _membersList.end());
}

bool Channel::getInviteOnly() const
{
	return _isInviteOnly;
}

void Channel::setInviteOnly(bool sign)
{
	_isInviteOnly = sign;
}

bool Channel::getTopicProtected() const
{
	return _isTopicProtected;
}

void Channel::setTopicProtected(bool sign)
{
	_isTopicProtected = sign;
}

void Channel::setKey(std::string key)
{
	_key = key;
}

void Channel::unsetKey()
{
	_key = "";
}

bool Channel::hasKey() const
{
	return !_key.empty();
}

const std::string& Channel::getKey() const
{
	return _key;
}

void Channel::setLimitUsers(int limit)
{
	_limitUsers = limit;
}

void Channel::unsetLimitUsers()
{
	_limitUsers = 0;
}

bool Channel::hasLimit() const
{
	return _limitUsers > 0;
}

int Channel::getLimitUsers() const
{
	return _limitUsers;
}

void Channel::addOperator(int fd)
{
	_operatorList.insert(fd);
}

void Channel::removeOperator(int fd)
{
	_operatorList.erase(fd);
}

bool Channel::isOperator(int fd) const
{
    return _operatorList.find(fd) != _operatorList.end();
}