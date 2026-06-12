#include <iostream>
#include "Parser.hpp"

int main()
{
    std::string rawPrompt = "JOIN #room1";
    IRCPrompt prompt = Parser::parsePrompt(rawPrompt);

    std::cout << "Prefix: " << prompt.prefix << "\n";
    std::cout << "Command: " << prompt.command << "\n";
    
    std::cout << "Args: ";
    for (size_t i = 0; i < prompt.args.size(); i++)
    {
        std::cout << prompt.args[i];
        if (i < prompt.args.size() - 1)
            std::cout << ", ";
    }
    std::cout << std::endl;

    return 0;
}