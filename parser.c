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

enum s_expr_type { SYMBOL, CELL, EMPTY_LIST };

/**
 * s_expr - A parse tree
 */
struct s_expr {
	union s_expr_value *value;
	enum s_expr_type type;
};

static int token_length;
static char *current_token;
static struct s_expr *empty_list;

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
	expr->value->symbol = (char *)
		malloc((token_length+1)*sizeof(char));
	strcpy(expr->value->symbol, symbol);
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

static struct s_expr *s_expr_as_empty_list()
{
	struct s_expr *expr = (struct s_expr *)
		malloc(sizeof(struct s_expr));
	expr->type = EMPTY_LIST;
	return expr;
}

void start_parser(int max_token_length)
{
	// Initialize lexer
	start_tokens(max_token_length);
	current_token = (char *) malloc((max_token_length+1) * sizeof(char));
	token_length = max_token_length;

	empty_list = s_expr_as_empty_list();
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

		while (1) {
			strcpy(current_token, get_token());
			if (!strcmp(current_token, ")"))
				break;
			struct cons_cell *next = new_cons_cell();

			curr->rest = s_expr_from_cons_cell(next);
			curr = next;
			curr->first = s_expression();
			next = new_cons_cell();
			curr->rest = s_expr_from_cons_cell(next);
		}
		// Terminate list with empty list
		next->rest = empty_list;
		return s_expr_from_cons_cell(first);
	} else if (!strcmp(current_token, "()")) {
		// It's a list of zero s_expressions (because the lexical
		// analyzer treats '()' as a single token)
		return empty_list;
	} else {
		return symbol();
	}
}

struct s_expr *get_expression(void)
{
	strcpy(current_token, get_token());
	return s_expression();
}
