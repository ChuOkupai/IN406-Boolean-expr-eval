#include <stdio.h>
#include <stdlib.h>

// Affiche une erreur en cas d'expression incorrecte
#define EXIT_INVALIDE() { puts("expression incorrecte"); exit(EXIT_FAILURE); }

// Ajoute un token à la file et déplace le pointeur à la fin
#define ENQUEUE(QUEUE, TYPE, VALEUR) ({ QUEUE->suivant = new_liste_token(TYPE, VALEUR); QUEUE = QUEUE->suivant; })

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

/// PARTIE DEBUG

// REMINDER: fonctions à supprimer AVANT le rendu

// Conversion d'un token en chaîne de charactères
const char*	toString(enum valeur v)
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

// Affiche la liste
void printList(liste_token l, char *name)
{
	printf("%s = ", name);
	if (! l)
	{
		puts("null");
		return;
	}
	while(l)
	{
		printf("%s", toString(l->valeur));
		l = l->suivant;
	}
	putchar('\n');
}

// Parcours prefixe
void	prefixe(arbre_token at, unsigned int p)
{
	if (! at)
		return;
	for (unsigned int i = 0; i < p; i++)
		printf("  ");
	printf("%s\n", toString(at->valeur));
	prefixe(at->gauche, ++p);
	prefixe(at->droite, p);
}

/// FIN PARTIE DEBUG

// Alloue la mémoire à un token dans la liste
liste_token	new_liste_token(enum type t, enum valeur v)
{
	liste_token l = (liste_token)malloc(sizeof(struct token));
	if (! l) exit(EXIT_FAILURE);
	l->type = t;
	l->valeur = v;
	l->suivant = NULL;
	return l;
}

// Libère la liste de tokens en mémoire
void	destroy_liste_token(liste_token l)
{
	if (! l)
		return;
	destroy_liste_token(l->suivant);
	free(l);
}

// Alloue la mémoire à un token dans l'arbre
arbre_token	new_arbre_token(liste_token l)
{
	arbre_token at = (arbre_token)malloc(sizeof(struct arbre));
	if (! at) exit(EXIT_FAILURE);
	at->type = l->type;
	at->valeur = l->valeur;
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

// Transforme une chaîne de charactères en liste de token
liste_token	string_to_token(const char *s)
{
	liste_token l = NULL, c = NULL;
	enum type t;
	enum valeur v;
	while (*s)
	{
		t = OPERATEUR;
		if (*s == ' ')		{ s++; continue; }
		else if (*s == '0')	{ t = CONSTANTE; v = FAUX; }
		else if (*s == '1')	{ t = CONSTANTE; v = VRAI; }
		else if (*s == '+')	{ v = OU; }
		else if (*s == '.')	{ v = ET; }
		else if (*s == '(')	{ t = PARENTHESE; v = GAUCHE; }
		else if (*s == ')')	{ t = PARENTHESE; v = DROITE; }
		else
		{
			if (s[0] == '=' && s[1] && s[1] == '>')
				v = IMPLICATION;
			else if (s[0] == '<' && s[1] && s[1] == '=' && s[2] && s[2] == '>')
			{
				v = EQUIVALENCE;
				s++;
			}
			else if (s[0] == 'N' && s[1] && s[1] == 'O' && s[2] && s[2] == 'N')
			{
				v = NON;
				s++;
			}
			else EXIT_INVALIDE(); // non reconnu
			s++;
		}
		if (! l)
		{
			l = new_liste_token(t, v);
			c = l; // pointeur sur l'élément actuel
		}
		else ENQUEUE(c, t, v);
		s++;
	}
	return l;
}

// Vérifie si l'expression est valide
int	est_valide(liste_token l)
{
	int q = 0, p = 0;
	
	while (l)
	{
		if (q) // état final
		{
			if (l->valeur == DROITE)
				p--;
			else
			{
				q = 0;
				if (l->type != OPERATEUR || l->valeur == NON)
					break;
			}
		}
		else // état initial
		{
			if (l->type == CONSTANTE)
				q = 1;
			else if (l->valeur == GAUCHE)
				p++;
			else if (l->valeur != NON)
				break;
		}
		l = l->suivant;
	}
	// vrai si état final et pas de parenthèses restantes dans la pile
	return q && ! p;
}

// Transforme une liste de tokens en arbre
arbre_token	liste_token_to_arbre_token(liste_token l)
{
	if (! l)
		return NULL;
	liste_token c = NULL, o = NULL, p;
	
	while (l)
	{
		p = l;
		l = l->suivant;
		if (p->type == CONSTANTE)
		{
			p->suivant = c;
			c = p;
		}
		else if (p->type == OPERATEUR || p->valeur == GAUCHE)
		{
			p->suivant = o;
			o = p;
		}
		else // (p->valeur == DROITE)
			free(p);
	}
	while (c || o)
	{
		printList(c, "c");
		printList(o, "o");
		printList(l, "l");
		if (! o) // une constante restante
		{
			p = l;
			l = c;
			c = c->suivant;
			l->suivant = p;
		}
		else if (o->valeur == GAUCHE)
		{
			p = o->suivant;
			free(o);
			o = p;
		}
		else
		{
			p = l;
			l = o;
			o = o->suivant;
			l->suivant = p;
			p = l;
			l = c;
			c = c->suivant;
			l->suivant = p;
		}
	}
	printList(c, "c");
	printList(o, "o");
	printList(l, "l");
	destroy_liste_token(l);
	return NULL;
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

int	main(int argc, char **argv)
{
	if (argc != 2)
		EXIT_INVALIDE();
	liste_token l = string_to_token(argv[1]);
	
	int v = est_valide(l);
	puts((v) ? "VRAI" : "FAUX");
	//if (! v) EXIT_INVALIDE();
	
	/*arbre_token a = liste_token_to_arbre_token(l);
	prefixe(a, 0);
	puts((arbre_to_int(a)) ? "VRAI" : "FAUX");
	destroy_arbre_token(a);*/
	return 0;
}