/**
 * parser.h - Interface for a parser for part of Scheme.
 */
#ifndef PARSER
#define PARSER
#include <stdlib.h>

/**
 * start_parser() - Initiates the parser
 * @token_length - max token length
 *
 * Call this function before get_expression, like this:
 *    start_parser();
 */
void start_parser(int token_length);

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
union s_expr *get_expression(void);

/**
 * free_parser() - Frees the memory consumed by the parser
 */
void free_parser(void);

#endif
