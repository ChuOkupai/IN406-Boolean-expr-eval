#include <stdio.h>
#include <stdlib.h>

// Affiche l'erreur et quitte le programme
#define exit_erreur() { perror("erreur"); exit(EXIT_FAILURE); }

// Affiche une erreur en cas d'expression incorrecte
#define exit_invalide() { fprintf(stderr, "expression incorrecte\n"); exit(EXIT_FAILURE); }

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

arbre_token	liste_token_to_arbre_token(liste_token l)
{
	if (! l)
		return NULL;
	arbre_token at2 = NULL;
	
	if (l->valeur == GAUCHE)
	{
		while ((l = l->suivant)->valeur == GAUCHE);
		at2 = liste_token_to_arbre_token(l);
		while ((l = l->suivant)->valeur != DROITE);
		while ((l = l->suivant) && l->valeur == DROITE);
		if (! l) // fin de l'expression
			return at2;
	}
	arbre_token at = new_arbre_token(l->valeur);
	l = l->suivant;
	if (at->type == CONSTANTE)
	{
		if (! l || l->valeur == DROITE) // fin de l'expression
			return at;
		if (l->type == OPERATEUR) // Opérateur après constante
		{
			arbre_token parent = liste_token_to_arbre_token(l);
			parent->gauche = at;
			at = parent;
		}
	}
	else if (at->type == OPERATEUR)
	{
		if (at->valeur == NON) // Cas particulier: opérateur avant constante
		{
			if (l->type == CONSTANTE)
				at->gauche = liste_token_to_arbre_token(l);
			else if (l->type == PARENTHESE)
			{
				while ((l = l->suivant)->valeur == GAUCHE);
				at->gauche = liste_token_to_arbre_token(l);
				while ((l = l->suivant)->valeur != DROITE);
				while ((l = l->suivant) && l->valeur == DROITE);
			}
			else
				exit_invalide();
		}
		else
		{
			at->gauche = at2;
			at->droite = liste_token_to_arbre_token(l);
		}
	}
	return at;
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
	// else (op == EQUIVALENCE)
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

const char*	getString(enum valeur v)
{
	if (v == FAUX)
		return "0";
	else if (v == VRAI)
		return "1";
	else if (v == NON)
		return "NON";
	else if (v == OU)
		return "+";
	else if (v == ET)
		return ".";
	else if (v == IMPLICATION)
		return "=>";
	else if (v == EQUIVALENCE)
		return "<=>";
	else if (v == GAUCHE)
		return "(";
	//else (v == DROITE)
	return ")";
}

// Parcours prefixe (debug)
void	prefixe(arbre_token at, unsigned int p)
{
	if (! at)
		return;
	for (unsigned int i = 0; i < p; i++)
		printf("  ");
	printf("%s\n", getString(at->valeur));
	prefixe(at->gauche, ++p);
	prefixe(at->droite, p);
}

// Transforme une chaîne de charactères en liste de token (debug)
liste_token	string_to_token_dumb(const char *s)
{
	liste_token l = NULL;
	enum valeur v;
	while (*s)
	{
		if (*s == '0')		v = FAUX;
		else if (*s == '1')	v = VRAI;
		else if (*s == '+')	v = OU;
		else if (*s == '.')	v = ET;
		else if (*s == '(')	v = GAUCHE;
		else if (*s == ')')	v = DROITE;
		else if (s[0] == '=' && s[1] && s[1] == '>')
		{
			v = IMPLICATION;
			s++;
		}
		else if (s[0] == '<' && s[1] && s[1] == '=' && s[2] && s[2] == '>')
		{
			v = EQUIVALENCE;
			s += 2;
		}
		else if (s[0] == 'N' && s[1] && s[1] == 'O' && s[2] && s[2] == 'N')
		{
			v = NON;
			s += 2;
		}
		else if (*s == ' ')
		{
			s++;
			continue;
		}
		else
			exit_invalide();
		l = insert_liste_token(l, v);
		s++;
	}
	return l;
}

// Test d'une expression booléenne (debug)
void	test_expression(const char *s)
{
	if (! s)
		return;
	liste_token l = string_to_token_dumb(s);
	arbre_token at = liste_token_to_arbre_token(l);
	prefixe(at, 0);
	printf((arbre_to_int(at)) ? "VRAI" : "FAUX");
	putchar('\n');
	destroy_arbre_token(at);
	destroy_liste_token(l);
}

int	main(int argc, char **argv)
{
	if (argc < 2)
		exit_invalide();
	test_expression(argv[1]);
	return 0;
}