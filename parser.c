/**
 * parser.c - Implements the interface given in parser.h.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lexer.h"
#include "parser.h"

static int token_length;
static char *current_token;
struct s_expr *empty_list;

struct s_expr *s_expr_from_boolean(int boolean)
{
	struct s_expr *expr = (struct s_expr *) malloc(sizeof(struct s_expr));

	expr->value = (union s_expr_value *) malloc(
		sizeof(union s_expr_value));
	expr->value->boolean = boolean;
	expr->type = BOOLEAN;
	return expr;
}

struct s_expr *s_expr_from_symbol(char *symbol)
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

struct s_expr *s_expr_from_cons_cell(struct cons_cell *cell)
{
	struct s_expr *expr = (struct s_expr *) malloc(sizeof(struct s_expr));

	expr->value = (union s_expr_value *) malloc(
		sizeof(union s_expr_value));
	expr->value->cell = cell;
	expr->type = CELL;
	return expr;
}

// Hidden; use the empty_list 'constant' instead.
static struct s_expr *s_expr_as_empty_list()
{
	struct s_expr *expr = (struct s_expr *)
		malloc(sizeof(struct s_expr));
	expr->type = EMPTY_LIST;
	return expr;
}

int is_empty_list(struct s_expr *expr)
{
	if (expr->type == BOOLEAN)
		return !expr->value->boolean;	// #f
	return expr->type == EMPTY_LIST;	// '()
}

int is_list(struct s_expr *expr)
{
	if (expr->type == CELL)
		return is_list(expr->value->cell->rest);

	return is_empty_list(expr);
}

struct s_expr *list_append(struct s_expr *ls, struct s_expr *value)
{
	struct cons_cell *new_cell = (struct cons_cell *)
		malloc(sizeof(struct cons_cell));
	new_cell->first = value;
	new_cell->rest = empty_list;
	struct s_expr *ls_end = ls;

	while (ls_end->type != EMPTY_LIST
	&& ls_end->value->cell->rest->type != EMPTY_LIST) {
		ls_end = ls_end->value->cell->rest;
	}
	struct cons_cell *last_cell = ls_end->value->cell;

	if (last_cell == NULL)
		return s_expr_from_cons_cell(new_cell);
	last_cell->rest = new_cell;
	return ls;
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

		struct cons_cell *first = (struct cons_cell *)
			malloc(sizeof(struct cons_cell));
		struct cons_cell *curr = first;
		struct cons_cell *prev_curr = NULL;

		while (1) {
			strcpy(current_token, get_token());
			if (!strcmp(current_token, ")"))
				break;
			curr->first = s_expression();
			struct cons_cell *next = (struct cons_cell *)
				malloc(sizeof(struct cons_cell));

			curr->rest = s_expr_from_cons_cell(next);
			prev_curr = curr;
			curr = next;
		}
		// Terminate list with empty list
		prev_curr->rest = empty_list;
		free(curr);
		return s_expr_from_cons_cell(first);
	} else if (!strcmp(current_token, "()")) {
		// It's a list of zero s_expressions (because the lexical
		// analyzer treats '()' as a single token)
		return empty_list;
	} else if (!strcmp(current_token, "#f")
	|| !strcmp(current_token, "#t")) {
		return s_expr_from_boolean(!strcmp(current_token, "#t"));
	} else {
		return symbol();
	}
}

struct s_expr *get_expression(void)
{
	strcpy(current_token, get_token());
	return s_expression();
}

static void _print_expression(struct s_expr *expr)
{
	if (expr->type == SYMBOL) {
		printf(expr->value->symbol);
	} else if (expr->type == CELL) {
		if (is_list(expr)) {
			printf("(");
			struct s_expr *curr = expr;

			while (curr->type != EMPTY_LIST) {
				_print_expression(curr->value->cell->first);
				curr = curr->value->cell->rest;
				if (curr->type != EMPTY_LIST)
					printf(" ");
			}
			printf(")");
		} else {
			printf("(");
			_print_expression(expr->value->cell->first);
			printf(" . ");
			_print_expression(expr->value->cell->rest);
			printf(")");
		}
	} else if (expr->type == BOOLEAN) {
		printf(expr->value->boolean ? "#t" : "#f");
	} else {
		// expr is the empty list
		printf("()");
	}
}

void print_expression(struct s_expr *expr)
{
	_print_expression(expr);
	printf("\n");
}
