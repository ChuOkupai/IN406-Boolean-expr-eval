#include <stdio.h>
#include <stdlib.h>

enum type
{
	CONSTANTE,
	OP_BINAIRE,
	OP_UNAIRE,
	PARENTHESE
};

enum valeur
{
	// Constantes
	FAUX = 0,
	VRAI = 1,
	
	// Opérateurs binaires
	OU,
	ET,
	IMPLICATION,
	EQUIVALENCE,

	// Opérateur unaire
	NON,

	// Paranthèses (priorités)
	GAUCHE,
	DROITE
};

typedef struct token* liste_token;
struct token
{
	enum type	type;
	enum valeur	valeur;
	liste_token	suivant;
};

typedef struct arbre* arbre_token;
struct arbre
{
	enum type	type;
	enum valeur	valeur;
	arbre_token	gauche;
	arbre_token	droite;
};

/*liste_token	string_to_token(char *string)
{

	return 0;
}*/

/*int	arbre_to_int(arbre_token at)
{

	return 0;
}*/

int	main(int argc, char **argv)
{
	if (argc < 2)
		return 0;
	(void)argv;
	return 0;
}