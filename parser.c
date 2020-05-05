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

struct s_expr *s_expr_from_integer(int integer)
{
	struct s_expr *expr = (struct s_expr *) malloc(sizeof(struct s_expr));

	expr->value = (union s_expr_value *) malloc(
		sizeof(union s_expr_value));
	expr->value->integer = integer;
	expr->type = INTEGER;
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

struct s_expr *s_expr_from_lambda(struct lambda *lmb)
{
	struct s_expr *expr = (struct s_expr *)
		malloc(sizeof(struct s_expr));

	expr->type = LAMBDA;
	expr->value = (union s_expr_value *) malloc(
		sizeof(union s_expr_value));
	expr->value->lambda = lmb;
	return expr;
}

struct s_expr *s_expr_from_builtin(struct builtin_function *builtin)
{
	struct s_expr *expr = (struct s_expr *)
		malloc(sizeof(struct s_expr));

	expr->type = BUILTIN;
	expr->value = (union s_expr_value *) malloc(
		sizeof(union s_expr_value));
	expr->value->builtin = builtin;
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

int list_length(struct s_expr *ls)
{
	int length = 0;

	while (!is_empty_list(ls)) {
		length++;
		ls = ls->value->cell->rest;
	}
	return length;
}

struct s_expr *list_append(struct s_expr *ls, struct s_expr *value)
{
	struct cons_cell *new_cell = (struct cons_cell *)
		malloc(sizeof(struct cons_cell));
	new_cell->first = value;
	new_cell->rest = empty_list;
	struct s_expr *new_cell_expr = s_expr_from_cons_cell(new_cell);
	struct s_expr *ls_end = ls;

	while (!is_empty_list(ls_end)
	&& !is_empty_list(ls_end->value->cell->rest)) {
		ls_end = ls_end->value->cell->rest;
	}

	if (is_empty_list(ls_end))
		return new_cell_expr;
	// If ls is a non-empty list, ls_end is a cons cell.
	struct cons_cell *last_cell = ls_end->value->cell;

	last_cell->rest = new_cell_expr;
	return ls;
}

int is_function(struct s_expr *expr)
{
	return expr->type == BUILTIN || expr->type == LAMBDA;
}

int equal(struct s_expr *a, struct s_expr *b)
{
	if (a->type != b->type)
		return 0;
	enum s_expr_type type = a->type;

	if (type == EMPTY_LIST)
		return 1;
	if (type == BOOLEAN)
		return a->value->boolean == b->value->boolean;
	if (type == SYMBOL)
		return !strcmp(a->value->symbol, b->value->symbol);
	// They're cons cells
	return equal(a->value->cell->first, b->value->cell->first)
		&& equal(a->value->cell->rest, b->value->cell->rest);
}

int is_assoc_list(struct s_expr *expr)
{
	if (!is_list(expr))
		return 0;
	struct s_expr *item = expr;

	// Check if each item is a list containing two elements.
	while (!is_empty_list(item)) {
		if (!is_list(item->value->cell->first)
		|| list_length(item->value->cell->first) != 2)
			return 0;
		item = item->value->cell->rest;
	}
	return 1;
}

struct s_expr *assoc_list_get(struct s_expr *expr, struct s_expr *key)
{
	struct s_expr *current = expr;

	while (!is_empty_list(current)) {
		struct s_expr *assoc = current->value->cell->first;

		if (equal(assoc->value->cell->first, key))
			return assoc;
		current = current->value->cell->rest;
	}
	return NULL;
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
	char *tmp = (char *) malloc(
		(strlen(current_token)+1) * sizeof(char));

	strcpy(tmp, current_token);
	int integer = strtol(tmp, &tmp, 10);

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
	} else if (*tmp == '\0') {
		// It's an integer (see top of function)
		return s_expr_from_integer(integer);
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

			while (!is_empty_list(curr)) {
				_print_expression(curr->value->cell->first);
				curr = curr->value->cell->rest;
				if (!is_empty_list(curr))
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
	} else if (expr->type == INTEGER) {
		printf("%d", expr->value->integer);
	} else if (expr->type == LAMBDA) {
		printf("<lambda %s>", expr->value->lambda->name);
	} else if (expr->type == BUILTIN) {
		printf("<built-in function %s>", expr->value->builtin->name);
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
