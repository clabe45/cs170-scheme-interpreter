/**************
 File: parser.h
 --------------
 Interface for a parser for part of Scheme.
 */
#ifndef PARSER
#define PARSER
#include <stdlib.h>

/***********************
 Function: startParser()
 ------------------------
 Initiates the parser

 Call this function before getExpression, like this:
    startParser();
 */
void startParser ();

/************************
 Function: getExpression()
 ------------------------
 Reads an s_expression from stdin and prints its parse tree.

 An s_expression takes the following form (in EBNF):
    <s_expression> = ( { <s_expression> } ) | #t | #f | <symbol> | ()

 Since it prints to stdout and reads from stdin, you can simply call:
    getExpression();
 */
void getExpression ();

#endif