/**
 * evaluator.h - Interface for executing an s-expression
 */

#ifndef EVAL
#define EVAL
#include <stdlib.h>

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
