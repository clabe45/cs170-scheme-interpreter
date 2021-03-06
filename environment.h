/**
 * environment.h - Stores user defined symbols
 */

#ifndef ENV
#define ENV
#include <stdlib.h>
#include "parser.h"

/**
 * init_env - Starts the environment
 */
void start_environment();

/**
 * set_env - Binds an s-expression to an id
 * @id - the name to bind to
 * @s_expr - the value to bind
 */
void set_env(char *id, struct s_expr *value);

/**
 * get_env - Gets the s-expression previously bound to `id`
 * @id - the id that was bound to
 * @returns the bound s-expression or NULL if no s-expression was bound
 */
struct s_expr *get_env(char *id);

/**
 * push_env - Pushes the current environment state onto the environment stack
 */
void push_env();

/**
 * pop_env - Pops the current environment state from the environment stack
 *
 * @returns 0 if there is only one state left, indicating that it failed, and 1
 * otherwise
 */
int pop_env();

#endif
