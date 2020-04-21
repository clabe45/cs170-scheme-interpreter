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

union s_expr_value {
	int boolean;
	char *symbol;
	struct cons_cell *cell;
};

enum s_expr_type { BOOLEAN, SYMBOL, CELL, EMPTY_LIST };

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
 * is_list - Determines if the s-expression is a proper list
 * @expr - the expression to test
 */
int is_list(struct s_expr *expr);

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
