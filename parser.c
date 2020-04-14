/**
 * parser.c - Implements the interface given in parser.h.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lexer.h"
#include "parser.h"

const struct cons_cell {
	union s_expr *first;
	union s_expr *rest;
};

/**
 * s_expr - A parse tree
 */
union s_expr {
	char *symbol;
	struct cons_cell *cell;
};

static char *current_token;

static struct cons_cell *new_cons_cell()
{
	struct cons_cell *cell = (struct cons_cell *)
		malloc(sizeof(struct cons_cell));
	return cell;
}

static union s_expr *s_expr_from_symbol(char *symbol)
{
	union s_expr *expr = (union s_expr *) malloc(sizeof(union s_expr));

	expr->symbol = symbol;
	return expr;
}

static union s_expr *s_expr_from_cons_cell(struct cons_cell *cell)
{
	union s_expr *expr = (union s_expr *) malloc(sizeof(union s_expr));

	expr->cell = cell;
	return expr;
}

void start_parser(int token_length)
{
	// Initialize lexer
	start_tokens(token_length);
	current_token = (char *) malloc((token_length+1) * sizeof(char));
}

void free_parser(void)
{
	free(current_token);
}

static union s_expr *symbol(void)
{
	union s_expr *expr = malloc(sizeof(union s_expr));

	expr->symbol = current_token;
	return expr;
}

static union s_expr *s_expression(void)
{
	if (!strcmp(current_token, "(")) {
		// Since it starts with (, it's a list of one or more
		// s_expressions
		strcpy(current_token, get_token());
		struct cons_cell *first = new_cons_cell();
		struct cons_cell *curr = first;

		curr->first = s_expression();
		struct cons_cell *next = new_cons_cell();

		curr->rest = s_expr_from_cons_cell(next);

		while (1) {
			strcpy(current_token, get_token());
			if (!strcmp(current_token, ")"))
				break;
			curr = next;
			curr->first = s_expression();
			next = new_cons_cell();
			curr->rest = s_expr_from_cons_cell(next);
		}
		// terminator node
		next->rest = NULL;
		return s_expr_from_cons_cell(first);
	} else if (!strcmp(current_token, "()")) {
		// It's a list of zero s_expressions (because the lexical
		// analyzer treats '()' as a single token)
		return NULL;	// terminater cons cell
	} else {
		return symbol();
	}
}

union s_expr *get_expression(void)
{
	strcpy(current_token, get_token());
	return s_expression();
}
