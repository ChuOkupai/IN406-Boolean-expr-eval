#include <stdio.h>
#include <stdlib.h>

// Affiche l'erreur et quitte le programme
#define exit_erreur() { perror("erreur"); exit(EXIT_FAILURE); }

// Affiche une erreur en cas d'expression incorrecte
#define exit_invalide() { fprintf(stderr, "erreur: expression incorrecte\n"); exit(EXIT_FAILURE); }

#define abr_gauche(ARBRE) (ARBRE) ? ;
#define abr_droite(ARBRE) 

enum type
{
	CONSTANTE,
	OPERATEUR,
	PARENTHESE
};

enum valeur
{
	// Constantes
	FAUX = 0,
	VRAI = 1,
	
	// Opérateur unaire
	NON,
	
	// Opérateurs binaires
	OU,
	ET,
	IMPLICATION,
	EQUIVALENCE,
	
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

enum type	get_type(enum valeur v)
{
	if (v == FAUX || v == VRAI)
		return CONSTANTE;
	return (v == GAUCHE || v == DROITE) ? PARENTHESE : OPERATEUR;
}

/*liste_token	string_to_token(char *string)
{

	return 0;
}*/

arbre_token	new_arbre_token(enum valeur v)
{
	arbre_token at = (arbre_token)malloc(sizeof(struct arbre));
	if (! at)
		exit_erreur();
	at->type = get_type(v);
	at->valeur = v;
	return at;
}

void	destroy_arbre_token(arbre_token at)
{
	if (! at)
		return;
	destroy_arbre_token(at->gauche);
	destroy_arbre_token(at->droite);
	free(at);
}

/*arbre_token	liste_token_to_arbre_token(liste_token l)
{
	
	return 0;
}*/

int	resoudre(enum valeur a, enum valeur b, enum valeur op)
{
	if (op == NON)
		return a ^ 1;
	else if (op == OU)
		return a | b;
	else if (op == ET)
		return a & b;
	else if (op == IMPLICATION)
		return (a ^ 1) | b;
	// else (op == EQUIVALENCE)
	return ((a ^ 1) | b) & ((b ^ 1) | a);
}

int	arbre_to_int(arbre_token at)
{
	if (! at)
		return 0;
	if (at->type == CONSTANTE)
		return at->valeur;
	return resoudre(arbre_to_int(at->gauche), arbre_to_int(at->droite), at->valeur);
}

int	main(int argc, char **argv)
{
	if (argc < 2)
		exit_invalide();
	arbre_token at = new_arbre_token(IMPLICATION);
	at->gauche = new_arbre_token(VRAI);
	at->droite = new_arbre_token(ET);
	at->droite->gauche = new_arbre_token(NON);
	at->droite->droite = new_arbre_token(VRAI);
	at->droite->gauche->gauche = new_arbre_token(OU);
	at->droite->gauche->gauche->gauche = new_arbre_token(VRAI);
	at->droite->gauche->gauche->droite = new_arbre_token(FAUX);
	printf((arbre_to_int(at)) ? "VRAI" : "FAUX");
	putchar('\n');
	destroy_arbre_token(at);
	(void)argv;
	return 0;
}