/**
 * parser.h - Interface for a parser for part of Scheme.
 */
#ifndef PARSER
#define PARSER
#include <stdlib.h>

const struct cons_cell {
	struct s_expr *first;
	struct s_expr *rest;
};

const struct lambda {
	char *name;
	char **args;
	int arg_count;
	struct s_expr *body;
};

struct builtin_function {
	char *name;
	// A function that takes any number of s_expr s and returns a single
	// s_expr.
	struct s_expr *(*function)(struct fn_arguments *args);
};

union s_expr_value {
	int boolean;
	int integer;
	char *symbol;
	struct cons_cell *cell;
	struct lambda *lambda;
	struct builtin_function *builtin;
};

enum s_expr_type { BOOLEAN, INTEGER, SYMBOL, CELL, EMPTY_LIST, LAMBDA, BUILTIN };

/**
 * s_expr - A parse tree
 */
struct s_expr {
	union s_expr_value *value;
	enum s_expr_type type;
};

/**
 * empty_list - Constant representing '()
 */
struct s_expr *empty_list;

/**
 * s_expr_from_boolean - Util method for creating a boolean s-expression
 * @boolean - the value of the new s-expression
 *
 * Creates an s_expr of type BOOLEAN with the value `boolean`.
 */
struct s_expr *s_expr_from_boolean(int boolean);

/**
 * s_expr_from_integer - Util method for creating an integer s-expression
 * @integer - the value of the new s-expression
 *
 * Creates an s_expr of type INTEGER with the value `integer`.
 */
struct s_expr *s_expr_from_integer(int integer);


/**
 * s_expr_from_symbol - Util method for creating a symbol s-expression
 * @symbol - the value of the new s-expression
 *
 * Creates an s_expr of type SYMBOL with the value `symbol`.
 */
struct s_expr *s_expr_from_symbol(char *symbol);

/**
 * s_expr_from_cons_cell - Util method for creating a cons-cell s-expression
 * @cell - the value of the new s-expression
 *
 * Creates an s_expr of type CELL with the value `cell`.
 */
struct s_expr *s_expr_from_cons_cell(struct cons_cell *cell);

/**
 * s_expr_from_lambda - Util method for creating a lambda s-expression
 * @lmb - the lambda object
 *
 * Creates an s_expr of type LAMBDA.
 */
struct s_expr *s_expr_from_lambda(struct lambda *lmb);

/**
 * s_expr_from_builtin - Util method for creating an s-expression for a builtin
 * function
 * @lmb - the lambda object
 *
 * Creates an s_expr of type BUILTIN.
 */
struct s_expr *s_expr_from_builtin(struct builtin_function *builtin);

/**
 * is_empty_list - Determines if the s-expression is the empty list
 * @expr - the expression to test
 */
int is_empty_list(struct s_expr *expr);

/**
 * is_list - Determines if the s-expression is a proper list
 * @expr - the expression to test
 */
int is_list(struct s_expr *expr);

/**
 * list_length - Returns the length of an s-expression list
 * @ls - the list
 */
int list_length(struct s_expr *expr);

/**
 * list_append - Adds an s-expression to a linked list
 * @ls - the list of s-expressions
 * @value - the item to append
 */
struct s_expr *list_append(struct s_expr *ls, struct s_expr *value);

/**
 * is_function - Determines if the s-expression is a builtin function or lambda
 * @expr - the expression to test
 */
int is_function(struct s_expr *expr);

/**
 * equal - Returns whether the two s-expressions have equal value
 * @a
 * @b
 */
int equal(struct s_expr *a, struct s_expr *b);

/**
 * is_assoc_list - Determines whether the given s-expression is an assocation list
 * @expr - the s-expression
 */
int is_assoc_list(struct s_expr *expr);

/**
 * assoc_list_get - Retrieves the value of `key` in `assoc_ls`
 * @assoc_ls - the association list
 * @key - the key of the assocation
 * @return - the value of the assocation, or NULL
 */
struct s_expr *assoc_list_get(struct s_expr *expr, struct s_expr *key);

/**
 * start_parser() - Initiates the parser
 * @token_length - max token length
 *
 * Call this function before get_expression, like this:
 *    start_parser();
 */
void start_parser(int token_length);

struct s_expr *empty_list;

/**
 * get_expression() - Reads an s_expression from stdin and prints its parse
 * tree.
 *
 * An s_expression takes the following form:
 *    <s_expression> = ( { <s_expression> } ) | #t | #f | <symbol> | ()
 *
 * Since it prints to stdout and reads from stdin, you can simply call:
 *    get_expression();
 */
struct s_expr *get_expression(void);

/**
 * free_parser() - Frees the memory consumed by the parser
 */
void free_parser(void);

/**
 * print_expression() - Displays the parse tree of an s-expression.
 * @expr
 */
void print_expression(struct s_expr *expr);

#endif
