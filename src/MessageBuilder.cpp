#include "MessageBuilder.hpp"

MessageBuilder::MessageBuilder(): _prefix(""), _command(""), _params(""), _content("")
{}

MessageBuilder::MessageBuilder(const std::string &command): _prefix(""), _command(command), _params(""), _content("")
{}

MessageBuilder::MessageBuilder(const MessageBuilder &src)
{
	*this = src;
}

MessageBuilder	&MessageBuilder::operator=(const MessageBuilder &src)
{
	if (this != &src)
	{
		_prefix = src._prefix;
		_command = src._command;
		_params = src._params;
		_content = src._content;
	}
	return (*this);
}

MessageBuilder::~MessageBuilder()
{}

// Les fonctions set sont particulieres car elle renvoie une reference vers l'objet lui meme,
// cela permet le chainage de methode, c'est a dire que l'on peut creer nos messages
// en les personnalisant

MessageBuilder	&MessageBuilder::setPrefix(const std::string &prefix)
{
	_prefix = ":" + prefix + " ";
	return (*this);
}

MessageBuilder	&MessageBuilder::setParam(const std::string &param)
{
	_params += " " + param;
	return (*this);
}

MessageBuilder	&MessageBuilder::setContent(const std::string &content)
{
	_content = " :" + content;
	return (*this);
}

// Cette fonction permet d'assembler le message une fois que les setter on finis leur travail
// Elle rajoute le \r\n a la fin de chaque message comme le veut RFC

std::string	MessageBuilder::build() const
{
	std::string	message = "";

	if (!_prefix.empty())
		message += _prefix;
	message += _command;
	if (!_params.empty())
		message += _params;
	if (!_content.empty())
		message += _content;
	message += "\r\n";
	return (message);
}