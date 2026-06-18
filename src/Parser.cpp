#include "Parser.hpp"

IRCPrompt Parser::parsePrompt(std::string raw_line)
{
	IRCPrompt prompted;

	if (!raw_line.empty() && raw_line[0] == ':')
    {
        size_t pos_space = raw_line.find(' ');
        if (pos_space != std::string::npos)
        {
            prompted.prefix = raw_line.substr(1, pos_space - 1);
            raw_line = raw_line.substr(pos_space + 1);
        }
    }

	size_t pos_last_param = raw_line.find(" :");
	std::string last_param = "";

	if (pos_last_param != std::string::npos)
	{
		last_param = raw_line.substr(pos_last_param + 2);
		raw_line = raw_line.substr(0, pos_last_param);
	}
	std::stringstream ss(raw_line);
	std::string word;

	if (ss >> word)
		prompted.command = word;

	while (ss >> word)
		prompted.args.push_back(word);

	if (pos_last_param != std::string::npos)
        prompted.args.push_back(last_param);

	return prompted;
}

Parser::~Parser()
{

}