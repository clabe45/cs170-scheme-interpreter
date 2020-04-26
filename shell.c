#define TOKEN_SIZE 20

/**
 * shell.c - The interactive shell
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parser.h"
#include "evaluator.h"

main(void)
{
	printf("A parser for a subset of Scheme. Type any Scheme");
	printf(" expression and its\n");
	printf("\"parse tree\" will be printed out. Type Ctrl-C to quit.\n");

	start_parser(TOKEN_SIZE);
	start_evaluator(TOKEN_SIZE);

	while (1) {
		printf("scheme> ");
		struct s_expr *input = get_expression();
		struct s_expr *result = eval_expression(input);
		print_expression(result);
	}
	free_parser();
}
