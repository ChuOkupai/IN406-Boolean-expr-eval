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

liste_token	new_liste_token(enum valeur v)
{
	liste_token l = (liste_token)malloc(sizeof(struct token));
	if (! l)
		exit_erreur();
	l->type = get_type(v);
	l->valeur = v;
	l->suivant = NULL;
	return l;
}

void	destroy_liste_token(liste_token l)
{
	if (! l)
		return;
	destroy_liste_token(l->suivant);
	free(l);
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
	at->gauche = NULL;
	at->droite = NULL;
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

// NE GERE PAS LES PARENTHESES
arbre_token	liste_token_to_arbre_token(liste_token l)
{
	if (! l)
		return NULL;
	arbre_token at = new_arbre_token(l->valeur);
	if (l->valeur == NON)
	{
		at->gauche = liste_token_to_arbre_token(l->suivant);
		// ???
		return at;
	}
	else if (! l->suivant)
		return at;
	arbre_token parent = new_arbre_token(l->suivant->valeur);
	parent->gauche = at;
	parent->droite = liste_token_to_arbre_token(l->suivant->suivant);
	return parent;
}

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
	else if (at->type == CONSTANTE)
		return at->valeur;
	return resoudre(arbre_to_int(at->gauche), arbre_to_int(at->droite), at->valeur);
}

void	infixe(arbre_token at)
{
	if (! at)
		return;
	infixe(at->gauche);
	char *buf;
	if (at->valeur == FAUX)
		buf = "0";
	else if (at->valeur == VRAI)
		buf = "1";
	else if (at->valeur == NON)
		buf = "NON";
	else if (at->valeur == OU)
		buf = "+";
	else if (at->valeur == ET)
		buf = ".";
	else if (at->valeur == IMPLICATION)
		buf = "=>";
	else if (at->valeur == EQUIVALENCE)
		buf = "<=>";
	printf("%s", buf);
	infixe(at->droite);
}

void	test_expression(arbre_token at)
{
	printf("expression = \"");
	infixe(at);
	printf("\"\n");
	printf((arbre_to_int(at)) ? "VRAI" : "FAUX");
	putchar('\n');
}

// (1=>(NON(1+0).1))
void	test1()
{
	arbre_token at = new_arbre_token(IMPLICATION);
	at->gauche = new_arbre_token(VRAI);
	at->droite = new_arbre_token(ET);
	at->droite->gauche = new_arbre_token(NON);
	at->droite->droite = new_arbre_token(VRAI);
	at->droite->gauche->gauche = new_arbre_token(OU);
	at->droite->gauche->gauche->gauche = new_arbre_token(VRAI);
	at->droite->gauche->gauche->droite = new_arbre_token(FAUX);
	test_expression(at);
	destroy_arbre_token(at);
}

// 1.0+1
void	test2()
{
	liste_token l = new_liste_token(VRAI);
	l->suivant = new_liste_token(ET);
	l->suivant->suivant = new_liste_token(FAUX);
	l->suivant->suivant->suivant = new_liste_token(OU);
	l->suivant->suivant->suivant->suivant = new_liste_token(VRAI);
	arbre_token at = liste_token_to_arbre_token(l);
	test_expression(at);
	destroy_liste_token(l);
	destroy_arbre_token(at);
}

int	main(int argc, char **argv)
{
	if (argc < 2)
		exit_invalide();
	(void)argv;
	test2();
	return 0;
}