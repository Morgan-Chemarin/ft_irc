#include <iostream>
#include "Parser.hpp"

int main()
{
    std::string rawPrompt = ":salut JOIN #general :Hello les developpeurs !";
    Parser::processclientCommand(rawPrompt);

    return 0;
}