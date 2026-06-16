#include "Channel.hpp"

Channel::Channel(): _name("")
{

}

Channel::Channel(const std::string& name): _name(name)
{

}

Channel::Channel(const Channel& other)
{
	_name = other._name;
	_membersList = other._membersList;
}

Channel& Channel::operator=(const Channel& other)
{
	if (this != &other)
	{
		_name = other._name;
		_membersList = other._membersList;
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
