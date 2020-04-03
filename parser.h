/**
 * parser.h - Interface for a parser for part of Scheme.
 */
#ifndef PARSER
#define PARSER
#include <stdlib.h>

/**
 * startParser() - Initiates the parser
 * @tokenLength - max token length
 *
 * Call this function before getExpression, like this:
 *    startParser();
 */
void startParser(int tokenLength);

/**
 * getExpression() - Reads an s_expression from stdin and prints its parse
 * tree.
 *
 * An s_expression takes the following form:
 *    <s_expression> = ( { <s_expression> } ) | #t | #f | <symbol> | ()
 *
 * Since it prints to stdout and reads from stdin, you can simply call:
 *    getExpression();
 */
void getExpression(void);

/**
 * freeParser() - Frees the memory consumed by the parser
 */
void freeParser(void);

#endif
