/**
 * evaluator.c - Interface for executing an s-expression
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parser.h"
#include "evaluator.h"

static struct fn_arguments {
	struct s_expr *value;
	struct fn_arguments *next;
};

static struct builtin_fn {
	char *name;
	// A function that takes any number of s_expr s and returns a single
	// s_expr.
	struct s_expr *(*function)(struct fn_arguments *args);
};

static struct builtin_fn_list {
	struct builtin_fn *value;
	struct builtin_fn_list *next;
};

// Start of linked list
static struct builtin_fn_list *builtin_functions;
// End of linked list
static struct builtin_fn_list *last_registered_function;

static void register_builtin_function(char *name,
struct s_expr *(*function)(struct fn_arguments *))
{
	struct builtin_fn *function_entry = (struct builtin_fn *)
		malloc(sizeof(struct builtin_fn));

	function_entry->name = (char *) malloc(
		(strlen(name)+1) * sizeof(char));
	strcpy(function_entry->name, name);
	function_entry->function = function;
	struct builtin_fn_list *function_entry_node = (struct builtin_fn_list *)
		malloc(sizeof(struct builtin_fn_list));

	function_entry_node->value = function_entry;
	function_entry_node->next = NULL;
	if (builtin_functions == NULL) {
		// Create first node in list.
		builtin_functions = function_entry_node;
		last_registered_function = function_entry_node;
	} else {
		// Append node to list.
		last_registered_function->next = function_entry_node;
		last_registered_function = function_entry_node;
	}
}

// BUILTIN FUNCTIONS

static struct s_expr *list(struct fn_arguments *args)
{
	struct s_expr *first = empty_list;
	struct s_expr *last = first;
	struct fn_arguments *arg = args;

	while (arg != NULL) {
		struct cons_cell *next = (struct cons_cell *)
			malloc(sizeof(struct cons_cell));

		next->first = eval_expression(arg->value);
		next->rest = empty_list;
		struct s_expr *next_s_expr = s_expr_from_cons_cell(next);

		if (last != empty_list) {
			last->value->cell->rest = next_s_expr;
			last = next_s_expr;
		} else {
			first = next_s_expr;
			last = next_s_expr;
		}
		arg = arg->next;
	}

	return first;
}

static struct s_expr *is_empty(struct fn_arguments *args)
{
	if (args == NULL || args->next != NULL) {
		// TODO error: arity mismatch
		return empty_list;
	}
	struct s_expr *value = eval_expression(args->value);

	return s_expr_from_boolean(is_empty_list(value));
}

static struct s_expr *append(struct fn_arguments *args)
{
	struct s_expr *result = empty_list;
	struct fn_arguments *arg = args;

	while (arg != NULL) {
		struct s_expr *curr = eval_expression(arg->value);

		if (is_list(curr)) {
			struct s_expr *curr_item = curr;

			while (!is_empty_list(curr_item)) {
				result = list_append(result,
					curr_item->value->cell->first);
				curr_item = curr_item->value->cell->rest;
			}
		} else {
			if (arg->next != NULL || arg == args) {
				// Only the last argument can be a non-list.
				// Additionally, the first argument must be a list.
				// TODO error: type mismatch (expecting list, found ...)
				return empty_list;
			}
			// result must be a non-empty list.
			// Make result an improper list.
			result->value->cell->rest = curr;
		}
		arg = arg->next;
	}
	return result;
}

static struct s_expr *quote(struct fn_arguments *args)
{
	if (args == NULL) {
		// TODO error: arity mismatch
		return empty_list;
	}

	if (args->next != NULL) {
		// TODO error: arity mismatch
		return args->value;
	}

	// Don't evaluate args->value; treat it as a literal.
	return args->value;
}

static struct s_expr *cons(struct fn_arguments *args)
{
	if (args == NULL || args->next == NULL || args->next->next != NULL) {
		// TODO error: arity mismatch
		return empty_list;
	}
	struct s_expr *first = eval_expression(args->value);
	struct s_expr *second = eval_expression(args->next->value);
	struct cons_cell *cell = (struct cons_cell *)
		malloc(sizeof(struct cons_cell));
	cell->first = first;
	cell->rest = second;

	return s_expr_from_cons_cell(cell);
}

static struct s_expr *car(struct fn_arguments *args)
{
	if (args == NULL) {
		// TODO: error: arity mismatch
		return empty_list;
	}
	struct s_expr *ls = eval_expression(args->value);

	if (args->next != NULL) {
		// TODO: error: arity mismatch
		return ls;
	}
	if (ls->type != CELL) {
		// TODO: error: contract violation
		return ls;
	}

	return ls->value->cell->first;
}

static struct s_expr *cdr(struct fn_arguments *args)
{
	if (args == NULL) {
		// TODO: error: arity mismatch
		return empty_list;
	}
	struct s_expr *ls = eval_expression(args->value);

	if (args->next != NULL) {
		// TODO: error: arity mismatch
		return ls;
	}
	if (ls->type != CELL) {
		// TODO: error: contract violation
		return ls;
	}

	return ls->value->cell->rest;
}

static struct s_expr *is_symbol(struct fn_arguments *args)
{
	if (args == NULL) {
		// TODO: error: arity mismatch
		return empty_list;
	}
	struct s_expr *ls = eval_expression(args->value);

	if (args->next != NULL) {
		// TODO: error: arity mismatch
		return ls;
	}
	return s_expr_from_boolean(args->value->type == SYMBOL);
}


void start_evaluator(void)
{
	register_builtin_function("list", list);
	register_builtin_function("empty?", is_empty);
	register_builtin_function("null?", is_empty);
	register_builtin_function("append", append);
	register_builtin_function("quote", quote);
	register_builtin_function("cons", cons);
	register_builtin_function("car", car);
	register_builtin_function("cdr", cdr);
	register_builtin_function("symbol?", is_symbol);
}

struct s_expr *eval_expression(struct s_expr *expr)
{
	if (!is_list(expr))
		return expr;
	if (is_empty_list(expr)) {
		// TODO error: missing procedure expression
		return expr;
	}
	struct s_expr *first = expr->value->cell->first; // name
	struct s_expr *item = expr->value->cell->rest; // args

	if (first->type != SYMBOL) {
		// TODO error: not a procedure
		return expr;
	}
	char *name = (char *) malloc(
		(strlen(first->value->symbol)+1) * sizeof(char));

	strcpy(name, first->value->symbol);
	struct fn_arguments *first_arg = NULL;
	struct fn_arguments *last_arg = first_arg;

	while (!is_empty_list(item)) {
		struct fn_arguments *new = (struct fn_arguments *)
			malloc(sizeof(struct fn_arguments));

		new->value = item->value->cell->first; // car
		new->next = NULL;
		if (first_arg == NULL) {
			first_arg = new;
			last_arg = new;
		} else {
			last_arg->next = new;
			last_arg = new;
		}

		item = item->value->cell->rest; // cdr
	}

	// Evaluate list by calling function
	struct builtin_fn_list *curr = builtin_functions;

	while (curr != NULL) {
		struct builtin_fn *builtin = curr->value;

		if (!strcmp(name, builtin->name))
			return (*(builtin->function))(first_arg);
		curr = curr->next;
	}

	// TODO: error: undefined
	return expr;
}
