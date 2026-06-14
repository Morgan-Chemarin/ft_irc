#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

struct IRCPrompt
{
	std::string prefix;
	std::string command;
	std::vector<std::string> args;
};

class Parser
{
	public:
		static IRCPrompt parsePrompt(std::string raw_line);

		~Parser();
	private:
		Parser();
		Parser(const Parser& other);
		Parser& operator=(const Parser& other);
};

#endif