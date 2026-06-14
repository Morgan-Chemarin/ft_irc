#include "Parser.hpp"

//? :prefixe <commande> <parametre_1> <parametre_2> ... :dernier_parametre
/*

":prefix" = commence toujours par : permet de savoir dou vient le message

"COMMANDE" = soit mot en majuscule (JOIN, MSGPRIV) soit un code a 3 chiffres, toujours separe des param avec 1 ou plusieurs space

"argument1,2,3" = des mots simples (un nom de salle a rejoindre, un nickname...)

" :dernier_parametre" = commence par un : après un espace. seul paramètre qui a le droit de contenir des espaces (ca va etre le msg dans un MSGPRIV)

*/

IRCPrompt Parser::parsePrompt(std::string raw_line)
{
	IRCPrompt prompted;

	// check prefix et lastparam tant quon est en string et les retirer de la string si trouve
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

	// on passe en ss pour vite parser les mots restant en retirant les espaces
	std::stringstream ss(raw_line);
	std::string word;

	if (ss >> word)
		prompted.command = word;

	while (ss >> word)
		prompted.args.push_back(word);

	if (pos_last_param != std::string::npos)
        prompted.args.push_back(last_param);

	//? test print struct
	std::cout << "Prefix: " << prompted.prefix << "\n";
    std::cout << "Command: " << prompted.command << "\n";
    std::cout << "Args: ";
    for (size_t i = 0; i < prompted.args.size(); i++)
    {
        std::cout << prompted.args[i];
        if (i < prompted.args.size() - 1)
            std::cout << ", ";
    }
    std::cout << std::endl;

	return prompted;
}

Parser::~Parser()
{

}