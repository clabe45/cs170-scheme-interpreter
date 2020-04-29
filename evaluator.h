/**
 * evaluator.h - Interface for executing an s-expression
 */

#ifndef EVAL
#define EVAL
#include <stdlib.h>

/**
 * get_eval_error() - Retrieves the last error message
 * @buffer - the destination buffer
 * @buffer_size - the size of the destination buffer
 * @returns 1 if the error message was copied successfully
 *   and 0 otherwise
 */
int get_eval_error(char *buffer, int buffer_size);

/**
 * start_evaluator() - Initiates the evaluator
 *
 * Run before all other function calls from this module.
 */
void start_evaluator(void);

/**
 * eval_expression() - Executes the s-expression and returns the result
 * @expr
 */
struct s_expr *eval_expression(struct s_expr *expr);

#endif
