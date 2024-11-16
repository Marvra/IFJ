/**
 * @file expr.h
 * @author Vojtech Panek, Jaroslav Doktor
 * @brief  header file for precedent analysis of expressions
 * @todo
 */
#ifndef EXPR_H
#define EXPR_H

#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "stack.h"
#include "symtable.h"
#include "token.h"
#include "linked_list.h"

#define PREC_TABLE_SIZE 7

typedef enum precTableTerm_t {
	TERM_plusMinus,
	TERM_mulDiv,
	TERM_leftBracket,
	TERM_rightBracket,
	TERM_relational,
	TERM_variable,
	TERM_stackEnd,
} precTableTerm_t;

typedef enum {
	RULE_plusMinus, // E -> E + E || E - E
	RULE_mulDiv, // E -> E * E || E / E
	RULE_brackets, // E -> (E)
	RULE_relational, // E -> E == E || E != E || E >= ...
	RULE_e, // E -> i
} precTableRule_t;

int expr_start(TokenList *list);

precTableTerm_t expr_getTermFromToken(Token *token);

#endif