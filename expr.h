/**
 * @file expr.h
 * @author Jaroslav Doktor, Martin Vrablec
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


int expr_start(ASTNode **root, TokenList **list, Tokentype topOnParserStack);
int checkForTop(TokenList *list, Tokentype topOnParserStack);

precTableTerm_t expr_getTermFromToken(Token *token);
int getIndexFromTerm(precTableTerm_t term);
int CheckRule(DLList* linked_list);
int CheckForEnd(DLList linked_list);
int listTopIndex(DLList linked_list);
int shouldSkipTerm(precTableTerm_t term);

void insert(ASTNode **root, Token* curr_token);
int getTokenPrecedance(Tokentype type);
int getAstPrecedance(ASTNode *root);
Operator getOperatorFromToken(Tokentype type);
void joinBrackets(ASTNode **root, ASTNode *brackets_root);

int checkForFunction(TokenList *list, ASTNode **root, char *funcId);
void tokenToParameter(Token *token, ASTNode **node);



#endif
