#include <stdio.h>
#include <stdlib.h>

// Affiche une erreur en cas d'expression incorrecte et retourne la valeur
#define RETURN_INVALIDE(VALUE) ({ puts("expression incorrecte"); return VALUE; })

// Si DEBUG est différent de 0, affiche plus d'informations à l'écran
#define DEBUG 1

enum type
{
	CONSTANTE,
	OPERATEUR,
	PARENTHESE
};

// PRIORITE : NON -> ET -> OU -> (=>, <=>)
// 1+1.0 <=> 1+(1.0)

enum valeur
{
	// Constantes
	FAUX = 0,
	VRAI = 1,
	
	// Opérateur unaire
	NON,
	
	// Opérateurs binaires
	ET,
	OU,
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
	printf("%s: ", name);
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
void	prefixe(arbre_token at, int p)
{
	if (! at)
		return;
	for (int i = 1; i < p; i++)
		printf("  ");
	if (p)
		printf("↳ ");
	printf("%s\n", toString(at->valeur));
	prefixe(at->gauche, ++p);
	prefixe(at->droite, p);
}

/// FIN PARTIE DEBUG

// Alloue la mémoire à un token dans la liste
liste_token	new_liste_token(enum type t, enum valeur v)
{
	liste_token l = (liste_token)malloc(sizeof(struct token));
	if (! l)
		exit(EXIT_FAILURE); // erreur malloc
	l->type = t;
	l->valeur = v;
	l->suivant = NULL;
	return l;
}

// Libère la liste de tokens en mémoire
void	destroy_liste_token(liste_token l)
{
	liste_token s;
	while (l)
	{
		s = l->suivant;
		free(l);
		l = s;
	}
}

// Alloue la mémoire à un token dans l'arbre
arbre_token	new_arbre_token(liste_token l)
{
	arbre_token at = (arbre_token)malloc(sizeof(struct arbre));
	if (! at)
		exit(EXIT_FAILURE); // erreur malloc
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
			else
			{
				destroy_liste_token(l); // non reconnu
				l = NULL;
				break;
			}
			s++;
		}
		if (l)
		{
			c->suivant = new_liste_token(t, v);
			c = c->suivant;
		}
		else
		{
			l = new_liste_token(t, v);
			c = l; // pointeur sur l'élément actuel
		}
		s++;
	}
	#if DEBUG
		printList(l, "liste de tokens");
	#endif
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

// Transforme une liste de tokens de infixe à postfixe
liste_token	liste_token_to_postfix(liste_token l)
{
	if (! l)
		return NULL;
	liste_token c, exp = NULL, cexp = NULL, op = new_liste_token(PARENTHESE, GAUCHE), p;
	
	while (l)
	{
		p = l->suivant;
		if (l->type == CONSTANTE)
		{
			// Ajout à l'expression
			if (exp)
			{
				cexp->suivant = l;
				cexp = cexp->suivant;
			}
			else
			{
				exp = l;
				cexp = l;
				exp->suivant = NULL;
			}
		}
		else if (l->type == OPERATEUR)
		{
			// Gestion de la priorité
			while (op->type == OPERATEUR && l->valeur >= op->valeur)
			{
				cexp->suivant = op;
				cexp = cexp->suivant;
				op = op->suivant;
			}
			// On ajoute l'opérateur à la pile
			c = op;
			op = l;
			op->suivant = c;
		}
		else // (l->type == PARENTHESE)
		{
			if (l->valeur == GAUCHE)
			{
				// Ajout à la pile
				c = op;
				op = l;
				op->suivant = c;
			}
			else // (l->valeur == DROITE)
			{
				while (op->valeur != GAUCHE)
				{
					cexp->suivant = op;
					cexp = cexp->suivant;
					op = op->suivant;
				}
				c = op->suivant;
				free(op);
				free(l);
				op = c;
			}
		}
		l = p;
	}
	while (op->valeur != GAUCHE)
	{
		cexp->suivant = op;
		cexp = cexp->suivant;
		op = op->suivant;
	}
	cexp->suivant = NULL;
	free(op);
	#if DEBUG
		printList(exp, "expression postfixe");
	#endif
	return exp;
}

// Transforme une liste de tokens en arbre
arbre_token	liste_token_to_arbre_token(liste_token l)
{
	liste_token p, parenthese;
	arbre_token a = NULL, t, g;
	
	l = liste_token_to_postfix(l); // conversion pour simplifier la construction
	parenthese = new_liste_token(PARENTHESE, GAUCHE);
	while (l)
	{
		t = new_arbre_token(l);
		p = l->suivant;
		free(l);
		l = p;
		if (t->type == CONSTANTE)
		{
			// on ajoute la constante à la pile d'arbres
			g = a;
			a = new_arbre_token(parenthese);
			a->gauche = g;
			a->droite = t;
		}
		else // (t->type == OPERATEUR)
		{
			// on fusionne 2 arbres avec l'opérateur, puis on le remet dans la pile
			if (t->valeur == NON)
			{
				t->gauche = a->droite;
				a->droite = t;
			}
			else
			{
				t->gauche = a->gauche->droite;
				t->droite = a->droite;
				a->gauche->droite = t;
				t = a;
				a = a->gauche;
				free(t);
			}
		}
	}
	free(parenthese);
	t = a;
	a = a->droite;
	free(t);
	#if DEBUG
		puts("arbre:");
		prefixe(a, 0);
	#endif
	return a;
}

// Résolution de a op b
int	resoudre(enum valeur a, enum valeur b, enum valeur op)
{
	if (op == NON)
		return ! a;
	else if (op == OU)
		return a | b;
	else if (op == ET)
		return a & b;
	else if (op == IMPLICATION)
		return (! a) | b;
	// else (op == EQUIVALENCE)
	return ! a ^ b;
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
		RETURN_INVALIDE(-1);
	liste_token l = string_to_token(argv[1]);
	
	if (! est_valide(l))
		RETURN_INVALIDE(-2);
	arbre_token a = liste_token_to_arbre_token(l);
	puts((arbre_to_int(a)) ? "VRAI" : "FAUX");
	destroy_arbre_token(a);
	return 0;
}