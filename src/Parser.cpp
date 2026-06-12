#include "Parser.hpp"

//? :prefixe <commande> <parametre_1> <parametre_2> ... :dernier_parametre
/*

":prefix" = commence toujours par : permet de savoir dou vient le message

"COMMANDE" = soit mot en majuscule (JOIN, MSGPRIV) soit un code a 3 chiffres, toujours separe des param avec 1 ou plusieurs space

"argument1,2,3" = des mots simples (un nom de salle a rejoindre, un nickname...)

":dernier_parametre" = commence par un : après un espace. seul paramètre qui a le droit de contenir des espaces (ca va etre le msg dans un MSGPRIV)

*/

IRCPrompt Parser::parsePrompt(std::string raw_line)
{
	std::string prefixe = "";
	std::string commande = "";
	std::vector<std::string> parametres;

	(void)raw_line;
	
	// check prefixe entre ":" et " "

	// check dernierpaeametre apres " :"

	// decouper le reste selon les espaces (en premier commande)

	// ajouter dernierparam dans parametres

	return IRCPrompt();
}

Parser::~Parser()
{

}