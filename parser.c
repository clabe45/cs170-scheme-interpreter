/**
 * parser.c - Implements the interface given in parser.h.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lexer.h"
#include "parser.h"

static int indentationLevel;
static char *currentToken;

static char *repeatChar(char c, int times)
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

void startParser(int tokenLength)
{
	// Initialize lexer
	startTokens(tokenLength);
	currentToken = (char *) malloc((tokenLength+1) * sizeof(char));
}

static void Symbol(void)
{
	char *indent = repeatChar(' ', 2*indentationLevel);

	if (currentToken[0] == '#' || currentToken[0] == '\'')
		return;

	printf("%s%s\n", indent, currentToken);
}

static void S_Expression(void)
{
	char *indent = (char *) malloc((2*indentationLevel+1) * sizeof(char));

	strcpy(indent, repeatChar(' ', 2*indentationLevel));
	printf("%sS_Expression\n", indent);
	indentationLevel++;

	if (!strcmp(currentToken, "#t")) {
		printf("%s#t\n", indent);
	} else if (!strcmp(currentToken, "#f")) {
		printf("%s#f\n", indent);
	} else if (!strcmp(currentToken, "(")) {
		// Since it starts with (, it's a list of one or more
		// s_expressions
		printf("%s(\n", indent);
		strcpy(currentToken, getToken());
		S_Expression();
		while (1) {
			strcpy(currentToken, getToken());
			if (!strcmp(currentToken, ")"))
				break;
			S_Expression();
		}
		printf("%s)\n", indent);
	} else if (!strcmp(currentToken, "()")) {
		// It's a list of zero s_expressions (because the lexical
		// analyzer treats '()' as a single token)
		printf("%s()\n", indent);
	} else {
		Symbol();
	}

	indentationLevel--;
}

void getExpression(void)
{
	strcpy(currentToken, getToken());
	S_Expression();
}
