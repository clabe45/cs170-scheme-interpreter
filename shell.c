#define TOKEN_SIZE 20

/**
 * shell.c - The interactive shell
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "environment.h"
#include "parser.h"
#include "evaluator.h"

main(void)
{
	printf("A parser for a subset of Scheme. Type any Scheme");
	printf(" expression and its\n");
	printf("\"parse tree\" will be printed out. Type Ctrl-C to quit.\n");

	start_environment();
	start_parser(TOKEN_SIZE);
	start_evaluator();

	while (1) {
		printf("scheme> ");
		struct s_expr *input = get_expression();
		struct s_expr *result = eval_expression(input);
		if (result == NULL) {
			char error[128];
			get_eval_error(error, 128);
			fprintf(stderr, error);
			fprintf(stderr, "\n");
		} else {
			print_expression(result);
		}
	}
	free_parser();
}
