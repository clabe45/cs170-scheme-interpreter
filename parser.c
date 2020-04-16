/**
 * parser.c - Implements the interface given in parser.h.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lexer.h"
#include "parser.h"

const struct cons_cell {
	struct s_expr *first;
	struct s_expr *rest;
};

union s_expr_value {
	char *symbol;
	struct cons_cell *cell;
};

enum s_expr_type { SYMBOL, CELL };

/**
 * s_expr - A parse tree
 */
struct s_expr {
	union s_expr_value *value;
	enum s_expr_type type;
};

static char *current_token;

static struct cons_cell *new_cons_cell()
{
	struct cons_cell *cell = (struct cons_cell *)
		malloc(sizeof(struct cons_cell));
	return cell;
}

static struct s_expr *s_expr_from_symbol(char *symbol)
{
	struct s_expr *expr = (struct s_expr *) malloc(sizeof(struct s_expr));

	expr->value = (union s_expr_value *) malloc(
		sizeof(union s_expr_value));
	expr->value->symbol = symbol;
	expr->type = SYMBOL;
	return expr;
}

static struct s_expr *s_expr_from_cons_cell(struct cons_cell *cell)
{
	struct s_expr *expr = (struct s_expr *) malloc(sizeof(struct s_expr));

	expr->value = (union s_expr_value *) malloc(
		sizeof(union s_expr_value));
	expr->value->cell = cell;
	expr->type = CELL;
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

static struct s_expr *symbol(void)
{
	return s_expr_from_symbol(current_token);
}

static struct s_expr *s_expression(void)
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

struct s_expr *get_expression(void)
{
	strcpy(current_token, get_token());
	return s_expression();
}
