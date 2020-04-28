/**
 * environment.c - Stores user defined symbols
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parser.h"
#include "environment.h"

/*
 * Implementation notes:
 *
 * The environment is stored in a pseudo-map - a linked list whose nodes
 * are added at the beginning (hiding previous nodes with the same id).
 */

static struct definition {
	char *id;
	struct s_expr *value;
	struct definition *next;
};

static struct definition *definitions;

void set_env(char *id, struct s_expr *value)
{
	// If id already exists, hide the previous value by prepending this
	// item.
	struct definition *def = (struct definition *)
		malloc(sizeof(struct definition));
	def->id = id;
	def->value = value;
	def->next = definitions;
	definitions = def;
}

struct s_expr *get_env(char *id)
{
	struct definition *curr = definitions;

	while (curr != NULL) {
		if (!strcmp(curr->id, id))
			return curr->value;
		curr = curr->next;
	}
	return NULL;
}
