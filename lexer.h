/**
 * lexer.h - Lexical analyser for Scheme
 *
 * This is the interface for a lexical analyzer for part of Scheme.  It has an
 * operation for initializing the stream of tokens and for getting the next
 * token.
 */
#ifndef LEXER
#define LEXER
#include <stdlib.h>

/**
 * start_tokens() - Initialize a token stream of tokens.
 * @max_length - the maximum token length
 *
 * Call this function before scanning for tokens. Simply call,
 *
 *    start_tokens(20);
 *
 * The argument signifies the fact that tokens will have the given maximum
 * length. Thus in the above statement tokens in the stream can be at most 20
 * characters long.
 */
void start_tokens(int max_length);

/**
 * get_token() - Return the next token in the token stream.
 *
 * It ignores all white space, including newlines. It returns the tokens "(",
 * ")", "#t", "#f", "'" (the single quote), and "()" (the empty list, which is
 * returned as the string "()"). All other strings of symbols with no white
 * space are regarded as symbols or literals, and are returned as strings. (For
 * ease in scanning, there is one exception: the "#" sign is excluded except at
 * the beginning of #t or #f.)
 *
 * To invoke this, one may, for example, declare a string variable
 * named token:
 *
 *     char *token;
 *
 * Then get_token() can be invoked as,
 *
 *     token = get_token();
 *
 * This technique is fine as long as you are quite sure you will only want to
 * store the value in token until the next call to get_token(), and no longer.
 *
 * IT IS SAFER to allocate token statically, as in,
 *
 *     char token[20];
 *
 * (assuming, for example, that tokens are <= 20 characters) and do a string
 * copy of the return value from get_token():
 *
 *     strcpy(token, get_token());
 *
 * rather than the above assignment. In this syntax the string copy acts much
 * more like a true assignment, and using get_token() you can store the next
 * token in another variable in the same way without losing the information in
 * token.
 *
 * WARNING: Tokens may be at most max_length characters long (see
 * start_tokens()). This function is not guaranteed to work for longer tokens,
 * although it may work in most cases.
 */
char *get_token();

#endif
