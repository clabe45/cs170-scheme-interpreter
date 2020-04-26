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
	struct s_expr *(*function)(struct fn_arguments *);
};

static struct builtin_fn_list {
	struct builtin_fn *value;
	struct builtin_fn_list *next;
};

// Start of linked list
static struct builtin_fn_list *builtin_functions = NULL;
// End of linked list
static struct builtin_fn_list *last_registered_function = NULL;

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
		builtin_functions = (struct builtin_fn_list *)
			malloc(sizeof(struct builtin_fn_list));
		builtin_functions->value = function_entry;
		builtin_functions->next = NULL;
	} else {
		// Append node to list.
		last_registered_function->next = function_entry_node;
	}
	last_registered_function = function_entry_node;
}

void start_evaluator(void)
{
}

struct s_expr *eval_expression(struct s_expr *expr)
{
	if (!is_list(expr))
		return expr;
	if (expr->type == EMPTY_LIST) {
		// TODO error: missing procedure expression
		return expr;
	}
	struct s_expr *first = expr->value->cell->first;
	// Consume first element in array
	struct s_expr *item = expr->value->cell->rest;

	if (first->type != SYMBOL) {
		// TODO error: not a procedure
		return expr;
	}
	char *name = (char *) malloc(
		(strlen(first->value->symbol)+1) * sizeof(char));

	strcpy(name, first->value->symbol);
	struct fn_arguments *first_arg = NULL;
	struct fn_arguments *last_arg = first_arg;

	while (item->type != EMPTY_LIST) {
		struct fn_arguments *new = (struct fn_arguments *)
			malloc(sizeof(struct fn_arguments));

		new->value = item->value->cell->first; // car
		new->next = NULL;
		if (first_arg == NULL) {
			first_arg = new;
			last_arg = new;
		} else {
			last_arg->next = new;
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
