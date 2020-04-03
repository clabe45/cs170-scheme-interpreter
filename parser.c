/**
 * parser.c - Implements the interface given in parser.h.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lexer.h"
#include "parser.h"

static int indentation_level;
static char *current_token;

static char *repeat_char(char c, int times)
{
	char *result = (char *) malloc((times+1) * sizeof(char));

	if (result == NULL) {
		fprintf(stderr, "error: Out of memory");
		exit(1);
	}

	int i;

	for (i = 0; i < times; i++)
		result[i] = c;
	return result;
}

void start_parser(int token_length)
{
	// Initialize lexer
	start_tokens(token_length);
	current_token = (char *) malloc((token_length+1) * sizeof(char));
}

void free_parser(void)
{
	free(current_token);
}

static void symbol(void)
{
	char *indent = repeat_char(' ', 2*indentation_level);

	if (current_token[0] == '#' || current_token[0] == '\'')
		return;

	printf("%s%s\n", indent, current_token);

	free(indent);
}

static void s_expression(void)
{
	char *indent = (char *) malloc((2*indentation_level+1) * sizeof(char));

	strcpy(indent, repeat_char(' ', 2*indentation_level));
	printf("%ss_expression\n", indent);
	indentation_level++;

	if (!strcmp(current_token, "#t")) {
		printf("%s#t\n", indent);
	} else if (!strcmp(current_token, "#f")) {
		printf("%s#f\n", indent);
	} else if (!strcmp(current_token, "(")) {
		// Since it starts with (, it's a list of one or more
		// s_expressions
		printf("%s(\n", indent);
		strcpy(current_token, get_token());
		s_expression();
		while (1) {
			strcpy(current_token, get_token());
			if (!strcmp(current_token, ")"))
				break;
			s_expression();
		}
		printf("%s)\n", indent);
	} else if (!strcmp(current_token, "()")) {
		// It's a list of zero s_expressions (because the lexical
		// analyzer treats '()' as a single token)
		printf("%s()\n", indent);
	} else {
		symbol();
	}

	free(indent);
	indentation_level--;
}

void get_expression(void)
{
	strcpy(current_token, get_token());
	s_expression();
}
