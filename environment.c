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
 * The environment state is stored in a pseudo-map - a linked list whose
 * nodes are added at the beginning (hiding previous nodes with the same
 * id).
 */

static struct definition {
	char *id;
	struct s_expr *value;
	struct definition *prev;
};

static struct env_state {
	struct definition *definitions;
	struct env_state *prev;
};

static struct env_state *state_stack;

void start_environment()
{
	push_env();
}

void push_env()
{
	struct env_state *prev = state_stack;
	state_stack = (struct env_state *) malloc(sizeof(struct env_state));

	state_stack->definitions = NULL;
	if (prev != NULL) {
		// copy parent state's definitions
		struct definition *tmp = prev->definitions;

		while (tmp != NULL) {
			set_env(tmp->id, tmp->value);
			tmp = tmp->prev;
		}
	}
	state_stack->prev = prev;
}

int pop_env()
{
	if (state_stack->prev == NULL)
		return 0;

	struct env_state *popped = state_stack;

	state_stack = state_stack->prev;
	free(popped);
	return 1;
}

void set_env(char *id, struct s_expr *value)
{
	// If id already exists, hide the previous value by prepending this
	// item.
	struct definition *def = (struct definition *)
		malloc(sizeof(struct definition));
	def->id = id;
	def->value = value;
	def->prev = state_stack->definitions;
	state_stack->definitions = def;
}

struct s_expr *get_env(char *id)
{
	struct definition *curr = state_stack->definitions;

	while (curr != NULL) {
		if (!strcmp(curr->id, id))
			return curr->value;
		curr = curr->prev;
	}
	return NULL;
}
