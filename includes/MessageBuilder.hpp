#ifndef MESSAGEBUILDER_HPP
#define MESSAGEBUILDER_HPP

#include <string>

class MessageBuilder
{
	public:
		MessageBuilder();
		MessageBuilder(const std::string &command);
		MessageBuilder(const MessageBuilder &src);
		MessageBuilder	&operator=(const MessageBuilder &src);
		~MessageBuilder();

		MessageBuilder	&setPrefix(const std::string &prefix);
		MessageBuilder	&setParam(const std::string &param);
		MessageBuilder	&setContent(const std::string &content);

		std::string	build() const;
	private:
		std::string	_prefix;
		std::string	_command;
		std::string	_params;
		std::string	_content;
};

#endif