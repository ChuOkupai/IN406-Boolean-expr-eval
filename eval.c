#include <stdio.h>
#include <stdlib.h>

// Affiche l'erreur et quitte le programme
#define exit_erreur() { perror("erreur"); exit(EXIT_FAILURE); }

// Affiche une erreur en cas d'expression incorrecte
#define exit_invalide() { fprintf(stderr, "erreur: expression incorrecte\n"); exit(EXIT_FAILURE); }

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

// Alloue la mémoire à un token dans la liste
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

// Insertion d'un token dans une liste de tokens
liste_token	insert_liste_token(liste_token l, enum valeur v)
{
	if (! l)
		return new_liste_token(v);
	liste_token l2 = l;
	while (l->suivant)
		l = l->suivant;
	l->suivant = new_liste_token(v);
	return l2;
}

// Libère la liste de tokens en mémoire
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

// Alloue la mémoire à un token dans l'arbre
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

// Libère l'arbre de tokens en mémoire
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
	if (! l->suivant)
		return at;
	arbre_token parent = new_arbre_token(l->suivant->valeur);
	if (l->valeur == NON) // Cas particulier: opérateur avant constante
	{
		at->gauche = parent; // le parent est le fils
		if (! l->suivant->suivant) // si il n'y a pas d'opérateur à la suite
			parent = at;
		else
		{
			parent = new_arbre_token(l->suivant->suivant->valeur);
			parent->gauche = at;
			parent->droite = liste_token_to_arbre_token(l->suivant->suivant->suivant);
		}
	}
	else // parent est un opérateur binaire
	{
		parent->gauche = at;
		parent->droite = liste_token_to_arbre_token(l->suivant->suivant);
	}
	return parent;
}

// Résolution de a op b
int	resoudre(enum valeur a, enum valeur b, enum valeur op)
{
	if (op == NON)
		return a ^ 1; // NON sur 1 bit
	if (op == OU)
		return a | b;
	if (op == ET)
		return a & b;
	if (op == IMPLICATION)
		return (a ^ 1) | b;
	// op == EQUIVALENCE
	return ((a ^ 1) | b) & ((b ^ 1) | a);
}

// Calcul du résultat de l'arbre de tokens
int	arbre_to_int(arbre_token at)
{
	if (! at)
		return 0;
	else if (at->type == CONSTANTE)
		return at->valeur;
	return resoudre(arbre_to_int(at->gauche), arbre_to_int(at->droite), at->valeur);
}

// Parcours prefixe (debug)
void	prefixe(arbre_token at, unsigned int p)
{
	if (! at)
		return;
	for (unsigned int i = 0; i < p; i++)
		printf("  ");
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
	printf("%s\n", buf);
	prefixe(at->gauche, ++p);
	prefixe(at->droite, p);
}

// Test d'une expression booléenne (debug)
void	test_expression(arbre_token at)
{
	prefixe(at, 0);
	printf((arbre_to_int(at)) ? "VRAI" : "FAUX");
	putchar('\n');
}

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
	puts("(1=>(NON(1+0).1))");
	test_expression(at);
	destroy_arbre_token(at);
}

void	test2()
{
	liste_token l = new_liste_token(VRAI);
	l = insert_liste_token(l, ET);
	l = insert_liste_token(l, FAUX);
	l = insert_liste_token(l, OU);
	l = insert_liste_token(l, VRAI);
	arbre_token at = liste_token_to_arbre_token(l);
	puts("1.0+1");
	test_expression(at);
	destroy_liste_token(l);
	destroy_arbre_token(at);
}

void	test3()
{
	liste_token l = new_liste_token(NON);
	l = insert_liste_token(l, VRAI);
	l = insert_liste_token(l, OU);
	l = insert_liste_token(l, VRAI);
	arbre_token at = liste_token_to_arbre_token(l);
	puts("NON1+1");
	test_expression(at);
	destroy_liste_token(l);
	destroy_arbre_token(at);
}

int	main(int argc, char **argv)
{
	if (argc < 2)
		exit_invalide();
	(void)argv;
	test3();
	return 0;
}