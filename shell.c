/**
 * shell.c - The interactive shell
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parser.h"

main(void)
{
	printf("A parser for a subset of Scheme. Type any Scheme");
	printf("expression and its\n");
	printf("\"parse tree\" will be printed out. Type Ctrl-C to quit.\n");

	start_parser(20);

	while (1) {
		printf("scheme> ");
		get_expression();
	}
	free_parser();
}
