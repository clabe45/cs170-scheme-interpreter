/**
 * evaluator.c - Interface for executing an s-expression
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parser.h"
#include "environment.h"
#include "evaluator.h"

static char *last_error_message;

static void set_error_message(char *message)
{
	if (last_error_message != NULL)
		free(last_error_message);
	last_error_message = (char *)
		malloc((strlen(message)+1) * sizeof(char));

	strcpy(last_error_message, message);
}

int get_eval_error(char *buffer, int buffer_size)
{
	if (strlen(last_error_message) > buffer_size)
		return 0;

	strcpy(buffer, last_error_message);
	return 1;
}

static struct fn_arguments {
	struct s_expr *value;
	struct fn_arguments *next;
};

static void register_builtin_function(char *name,
struct s_expr *(*function)(struct fn_arguments *))
{
	struct builtin_function *function_entry = (struct builtin_function *)
		malloc(sizeof(struct builtin_function));

	function_entry->name = (char *) malloc(
		(strlen(name)+1) * sizeof(char));
	strcpy(function_entry->name, name);
	function_entry->function = *function;

	set_env(name, s_expr_from_builtin(function_entry));
}

// BUILTIN FUNCTIONS

static struct s_expr *exit_(struct fn_arguments *args)
{
	if (args != NULL) {
		set_error_message("exit - arity mismatch");
		return NULL;
	}

	exit(0);
}

static struct s_expr *list(struct fn_arguments *args)
{
	struct s_expr *first = empty_list;
	struct s_expr *last = first;
	struct fn_arguments *arg = args;

	while (arg != NULL) {
		struct cons_cell *next = (struct cons_cell *)
			malloc(sizeof(struct cons_cell));

		next->first = eval_expression(arg->value);
		if (next->first == NULL) return NULL;
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

static struct s_expr *is_list_(struct fn_arguments *args)
{
	if (args == NULL || args->next != NULL) {
		set_error_message("list? - arity mismatch");
		return NULL;
	}
	struct s_expr *val = eval_expression(args->value);

	if (val == NULL) return NULL;
	return s_expr_from_boolean(
		is_list(val));
}

static struct s_expr *is_empty(struct fn_arguments *args)
{
	if (args == NULL || args->next != NULL) {
		set_error_message("null? - arity mismatch");
		return NULL;
	}
	struct s_expr *value = eval_expression(args->value);
	if (value == NULL) return NULL;

	return s_expr_from_boolean(is_empty_list(value));
}

static struct s_expr *append(struct fn_arguments *args)
{
	struct s_expr *result = empty_list;
	struct fn_arguments *arg = args;

	while (arg != NULL) {
		struct s_expr *curr = eval_expression(arg->value);
		if (curr == NULL) return NULL;

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
				// Additionally, the first argument must be a
				// list.
				set_error_message(
					"append - type mismatch (expecting list)");
				return NULL;
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
	if (args == NULL || args->next != NULL) {
		set_error_message("quote - arity mismatch");
		return NULL;
	}

	// Don't evaluate args->value; treat it as a literal.
	return args->value;
}

static struct s_expr *cons(struct fn_arguments *args)
{
	if (args == NULL || args->next == NULL || args->next->next != NULL) {
		set_error_message("cons - arity mismatch");
		return NULL;
	}
	struct s_expr *first = eval_expression(args->value);
	struct s_expr *second = eval_expression(args->next->value);
	if (first == NULL || second == NULL) return NULL;

	struct cons_cell *cell = (struct cons_cell *)
		malloc(sizeof(struct cons_cell));
	cell->first = first;
	cell->rest = second;

	return s_expr_from_cons_cell(cell);
}

static struct s_expr *car(struct fn_arguments *args)
{
	if (args == NULL || args->next != NULL) {
		set_error_message("car - arity mismatch");
		return NULL;
	}
	struct s_expr *ls = eval_expression(args->value);
	if (ls == NULL) return NULL;

	if (ls->type != CELL) {
		set_error_message("car - type error (expected cons cell)");
		return NULL;
	}

	return ls->value->cell->first;
}

static struct s_expr *cdr(struct fn_arguments *args)
{
	if (args == NULL || args->next != NULL) {
		set_error_message("cdr - arity mismatch");
		return NULL;
	}
	struct s_expr *ls = eval_expression(args->value);
	if (ls == NULL) return NULL;

	if (ls->type != CELL) {
		set_error_message("cdr - type error (expected cons cell)");
		return NULL;
	}

	return ls->value->cell->rest;
}

static struct s_expr *add(struct fn_arguments *args)
{
	int sum = 0;
	struct fn_arguments *arg = args;

	while (arg != NULL) {
		struct s_expr *val = eval_expression(arg->value);
		if (val->type != INTEGER) {
			set_error_message("+ - type error (expected integer)");
			return NULL;
		}
		sum += val->value->integer;
		arg = arg->next;
	}
	return s_expr_from_integer(sum);
}

static struct s_expr *subtract(struct fn_arguments *args)
{
	if (args == NULL) {
		set_error_message("- - arity mismatch");
		return NULL;
	}
	struct s_expr *first = eval_expression(args->value);

	if (first->type != INTEGER) {
		set_error_message("- - type error (expected integer)");
		return NULL;
	}
	if (args->next == NULL) {
		return s_expr_from_integer(
			- first->value->integer
		);
	}
	// Subtract the rest from the first.
	int difference = 0;
	struct fn_arguments *arg = args->next;

	difference += first->value->integer;
	while (arg != NULL) {
		struct s_expr *curr = eval_expression(arg->value);

		if (curr->type != INTEGER) {
			set_error_message("- - type error (expected integer)");
			return NULL;
		}
		difference -= curr->value->integer;
		arg = arg->next;
	}
	return s_expr_from_integer(difference);
}

static struct s_expr *multiply(struct fn_arguments *args)
{
	int product = 1;
	struct fn_arguments *arg = args;

	while (arg != NULL) {
		struct s_expr *val = eval_expression(arg->value);
		if (val->type != INTEGER) {
			set_error_message("* - type error (expected integer)");
			return NULL;
		}
		product *= val->value->integer;
		arg = arg->next;
	}
	return s_expr_from_integer(product);
}

static struct s_expr *and(struct fn_arguments *args)
{
	if (args == NULL) {
		// no arguments; return #t
		return s_expr_from_boolean(1);
	}

	// Return the last truthy value, or #f
	struct fn_arguments *arg = args;
	struct s_expr *val;

	while (arg != NULL) {
		val = eval_expression(arg->value);

		if (is_empty_list(val))
			return s_expr_from_boolean(0);
		arg = arg->next;
	}
	return val;
}

static struct s_expr *or(struct fn_arguments *args)
{
	// return the first truthy value
	struct fn_arguments *arg = args;

	while (arg != NULL) {
		struct s_expr *val = eval_expression(arg->value);

		if (!is_empty_list(val))
			return val;
		arg = arg->next;
	}
	// no arguments; return #f
	return s_expr_from_boolean(0);
}

static struct s_expr *is_symbol(struct fn_arguments *args)
{
	if (args == NULL || args->next != NULL) {
		set_error_message("symbol? - arity mismatch");
		return NULL;
	}
	struct s_expr *ls = eval_expression(args->value);
	if (ls == NULL) return NULL;

	return s_expr_from_boolean(ls->type == SYMBOL);
}

static struct s_expr *are_equal(struct fn_arguments *args)
{
	if (args == NULL || args->next == NULL || args->next->next != NULL) {
		set_error_message("equal? - arity mismatch");
		return NULL;
	}
	struct s_expr *a = eval_expression(args->value);
	struct s_expr *b = eval_expression(args->next->value);

	if (a == NULL || b == NULL) return NULL;
	return s_expr_from_boolean(equal(a, b));
}

static struct s_expr *assoc(struct fn_arguments *args)
{
	if (args == NULL || args->next == NULL || args->next->next != NULL) {
		set_error_message("assoc - arity mismatch");
		return NULL;
	}
	struct s_expr *key = eval_expression(args->value);
	struct s_expr *assoc_list = eval_expression(args->next->value);

	if (key == NULL || assoc_list == NULL)
		return NULL;
	if (!is_assoc_list(assoc_list)) {
		set_error_message(
			"assoc - type error (expecting associative list)");
		return assoc_list;
	}
	struct s_expr *result = assoc_list_get(assoc_list, key);

	if (result == NULL)
		return s_expr_from_boolean(0);
	return result;
}

static struct s_expr *cond(struct fn_arguments *args)
{
	if (args == NULL) {
		// TODO return #<void>
		return empty_list;
	}

	while (args != NULL) {
		struct s_expr *clause = args->value;

		if (!is_list(clause)) {
			set_error_message("cond - type error (expected list)");
			return NULL;
		}
		if (list_length(clause) < 2) {
			set_error_message(
				"cond - value error (expected at least two elements in clause)"
			);
			return NULL;
		}
		struct s_expr *test = clause->value->cell->first;
		int else_clause = test->type == SYMBOL
			&& !strcmp(test->value->symbol, "else");

		if (!else_clause) {
			test = eval_expression(test);
			if (test == NULL) return NULL;
		}
		// Pass as long as `test` is not #f or '().
		int test_passed = !is_empty_list(test);
		struct s_expr *then_bodies = clause->value->cell->rest;

		if (test_passed || else_clause) {
			if (else_clause && args->next != NULL) {
				set_error_message(
					"cond - syntax error (else must be the last clause)");
				return NULL;
			}
			// Evaluate then bodies in order and return the result
			// of the last one.
			while (!is_empty_list(then_bodies)) {
				struct s_expr *result = eval_expression(
					then_bodies->value->cell->first);
				struct s_expr *rest =
					then_bodies->value->cell->rest;

				if (result == NULL) return NULL;
				if (is_empty_list(rest))
					return result;
				then_bodies = rest;
			}
			// Since there are at least two elements, this comment
			// will never be reached.
		}
		args = args->next;
	}
	// TODO return #<void>
	return empty_list;
}

struct s_expr *lambda_(struct fn_arguments *args)
{
	if (args == NULL || args->next == NULL
	|| args->next->next != NULL) {
		set_error_message("lambda - arity mismatch");
		return NULL;
	}
	struct s_expr *arg_names = args->value;
	struct s_expr *body = args->next->value; // don't evaluate

	if (!is_list(arg_names)) {
		set_error_message("lambda - type error (arguments must be a list)");
		return NULL;
	}
	int arg_count = list_length(arg_names);
	char **arg_list = (char **) malloc(
		arg_count * (sizeof(char *)));
	struct s_expr *tmp = arg_names;
	int i = 0;

	while (!is_empty_list(tmp)) {
		struct s_expr *arg = tmp->value->cell->first;

		if (arg->type != SYMBOL) {
			set_error_message(
				"lambda - type error (each argument must be a symbol)");
			return NULL;
		}
		arg_list[i] = (char *) malloc(
			(strlen(arg->value->symbol)+1) * sizeof(char));
		strcpy(arg_list[i], arg->value->symbol);
		tmp = tmp->value->cell->rest;
		i++;
	}
	struct lambda *lmb = (struct lambda *)
		malloc(sizeof(struct lambda));

	lmb->name = (char *) malloc(
		(strlen("anonymous")+1) * sizeof(char));
	strcpy(lmb->name, "anonymous");
	lmb->args = arg_list;
	lmb->arg_count = arg_count;
	lmb->body = body;
	return s_expr_from_lambda(lmb);
}

struct s_expr *define_(struct fn_arguments *args)
{
	if (args == NULL || args->next == NULL
	|| args->next->next != NULL) {
		set_error_message("define - arity mismatch");
		return NULL;
	}

	if (args->value->type == SYMBOL) {
		struct s_expr *id = args->value;
		struct s_expr *value = eval_expression(args->next->value);

		if (value == NULL) return NULL;
		set_env(id->value->symbol, value);
		return id;
	}

	if (is_list(args->value)) {
		struct s_expr *id = args->value->value->cell->first;
		if (id->type != SYMBOL) {
			set_error_message("define - type error (expected symbol)");
			return NULL;
		}
		struct s_expr *curr_arg = args->value->value->cell->rest;
		int arg_count = list_length(curr_arg);
		char **arg_list = (char **) malloc(
			arg_count * sizeof(char *));
		int i = 0;

		while (!is_empty_list(curr_arg)) {
			struct s_expr *tmp = curr_arg->value->cell->first;

			if (tmp->type != SYMBOL) {
				set_error_message("define - type error (expected symbol)");
				return NULL;
			}
			arg_list[i] = (char *) malloc(
				(strlen(tmp->value->symbol)+1) * sizeof(char));
			strcpy(arg_list[i], tmp->value->symbol);
			curr_arg = curr_arg->value->cell->rest;
			i++;
		}
		struct s_expr *body = args->next->value;

		struct lambda *lmb = (struct lambda *)
			malloc(sizeof(struct lambda));

		lmb->name = (char *) malloc(
			(strlen(id->value->symbol)+1) * sizeof(char));
		strcpy(lmb->name, id->value->symbol);
		lmb->args = arg_list;
		lmb->arg_count = arg_count;
		lmb->body = body;
		set_env(
			id->value->symbol,
			s_expr_from_lambda(lmb));
		return id;
	}

	set_error_message("define - type error (expecting symbol or list)");
	return NULL;
}

static struct s_expr *is_function_(struct fn_arguments *args)
{
	if (args == NULL || args->next != NULL) {
		set_error_message("function? - arity mismatch");
		return NULL;
	}
	struct s_expr *val = eval_expression(args->value);

	return s_expr_from_boolean(
		is_function(val));
}


void start_evaluator(void)
{
	register_builtin_function("exit", exit_);
	register_builtin_function("list", list);
	register_builtin_function("list?", is_list_);
	register_builtin_function("empty?", is_empty);
	register_builtin_function("null?", is_empty);
	register_builtin_function("append", append);
	register_builtin_function("quote", quote);
	register_builtin_function("cons", cons);
	register_builtin_function("car", car);
	register_builtin_function("cdr", cdr);
	register_builtin_function("+", add);
	register_builtin_function("-", subtract);
	register_builtin_function("*", multiply);
	register_builtin_function("and", and);
	register_builtin_function("or", or);
	register_builtin_function("symbol?", is_symbol);
	register_builtin_function("equal?", are_equal);
	register_builtin_function("assoc", assoc);
	register_builtin_function("cond", cond);
	register_builtin_function("lambda", lambda_);
	register_builtin_function("define", define_);
	register_builtin_function("function?", is_function_);
}

static struct fn_arguments *read_arguments(struct s_expr *start)
{
	struct s_expr *curr = start;
	struct fn_arguments *first_arg = NULL;
	struct fn_arguments *last_arg = first_arg;

	while (!is_empty_list(curr)) {
		struct fn_arguments *new = (struct fn_arguments *)
			malloc(sizeof(struct fn_arguments));

		new->value = curr->value->cell->first; // car
		new->next = NULL;
		if (first_arg == NULL) {
			first_arg = new;
			last_arg = new;
		} else {
			last_arg->next = new;
			last_arg = new;
		}

		curr = curr->value->cell->rest; // cdr
	}
	return first_arg;
}

static struct s_expr *eval_list(struct s_expr *expr)
{
	if (is_empty_list(expr)) {
		set_error_message("syntax error (missing procedure expression)");
		return NULL;
	}
	struct s_expr *first = eval_expression(expr->value->cell->first); // name or lambda
	struct s_expr *rest = expr->value->cell->rest; // args

	if (first == NULL) return NULL;
	if (!is_function(first)) {
		set_error_message("type error (expected function)");
		return NULL;
	}
	struct fn_arguments *args = read_arguments(rest);

	if (first->type == BUILTIN) {
		first->value->builtin->function(args);
	} else {
		// first is a lambda expression
		struct lambda *lmb = first->value->lambda;

		// Prepare local environment
		push_env();
		int i;
		struct fn_arguments *arg = args;

		for (i = 0; i < lmb->arg_count; i++) {
			if (arg == NULL) {
				// Not enough arguments passed to lambda
				pop_env();
				set_error_message("lambda - arity mismatch");
				return NULL;
			}
			set_env(lmb->args[i], eval_expression(arg->value));
			arg = arg->next;
		}
		if (arg != NULL) {
			// Too many arguments passed to lambda
			pop_env();
			set_error_message("lambda - arity mismatch");
			return NULL;
		}

		// Evaluate function body
		struct s_expr *ret = eval_expression(lmb->body);

		pop_env();
		return ret;
	}
}

static struct s_expr *eval_symbol(struct s_expr *expr)
{
	struct s_expr *value = get_env(expr->value->symbol);

	if (value == NULL) {
		set_error_message("reference error (undefined symbol)");
		return NULL;
	}
	return value;
}

struct s_expr *eval_expression(struct s_expr *expr)
{
	// Only 'call' lists, not booleans
	if (is_list(expr) && expr->type != BOOLEAN)
		return eval_list(expr);
	if (expr->type == SYMBOL)
		return eval_symbol(expr);

	// Return itself
	return expr;
}
