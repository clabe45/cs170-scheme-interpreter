/**
 * lexer.c - See header file for more information.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "lexer.h"

// the current token
static char *lexeme;
// the current character in the input stream from the keyboard
static char c;
// if the previous call to get_token() required looking ahead
static int lookahead;

/**
 * new_token() - Reinitializes lexeme to a string of max_length.
 *
 * De-allocates the existing lexeme and allocates a new one (a
 * max_length-character array). This is called when start_tokens() is called. The
 * variable lexeme remains as a dynamically allocated array. If a new token
 * stream is desired, the lexeme array is re-allocated.
 */
static void new_token(int max_length)
{
	if (lexeme != NULL)
		free(lexeme);

	lexeme = (char *) calloc(max_length, sizeof(char));
	if (lexeme == NULL) {
		printf("Out of memory, too many tokens.\n");
		exit(0);
	}
}

/**
 * start_tokens()
 */
void start_tokens(int max_length)
{
	lookahead = 0;
	lexeme = NULL;
	new_token(max_length);
}

/**
 * get_token()
 *
 * Implementation notes: The function works by getting the first character, in
 * case the previous call required lookahead, then skipping over whitespace. The
 * main part is the "if" statement that handles 4 cases:
 *   (1) Current character is ")" or "'" (single quote). Then return the
 *       character as a string.
 *   (2) Current character is "(". Then scan for ")". If found, return "()".
 *       Otherwise, return "(".
 *   (3) Current character is "#". Only accepted following characters are t
 *       and f, in which case "#t" or "#f" are returned.
 *   (4) Default case: Scan for a string of characters, and return as a string
 *       (a string cannot contain any whitespace character, or "(" or ")").
 */
char *get_token()
{
	int i; //local index for lexeme

	if (!lookahead) //get first char
		c = getchar();

	while ((c == ' ') || (c == '\n')) //skip white space
		c = getchar();

	if ((c == ')') || (c == '\'')) { //Case (1): right paren or quote
		lexeme[0] = c;
		lexeme[1] = '\0';
		lookahead = 0;
	} else if (c == '(') { //Case (2): left paren or ()
		lookahead = 1;
		c = getchar();
		while ((c == ' ') || (c == '\n'))
			c = getchar();
		if (c == ')') {
			strcpy(lexeme, "()"); //empty list token
			lookahead = 0;
		} else {
			strcpy(lexeme, "(");
		}
	} else if (c == '#') { //Case (3): #t or #f
		lookahead = 0;
		c = getchar();
		if ((c != 't') && (c != 'f')) {
			printf("Illegal symbol after #.\n");
			exit(1);
		}
		if (c == 't')
			strcpy(lexeme, "#t");
		else
			strcpy(lexeme, "#f");
	} else { //Case (4): scan for symbol
		i = 0;
		lookahead = 1;
		while ((c != '(') && (c != ')') && (c != ' ') && (c != '\n')) {
			lexeme[i++] = c;
			c = getchar();
		}
		lexeme[i] = '\0';
	}

	return lexeme;
}
