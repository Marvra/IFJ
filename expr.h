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

#define PREC_TABLE_SIZE 14

typedef enum
{
	ACTION_shift,
	ACTION_reduce,
	ACTION_illegal,
	ACTION_DONE	
}	precTableAction_t;

typedef enum
{
	// Relační operátory
	TERM_plus,		
	TERM_minus,		
	TERM_mul,		
	TERM_div,

	// ()	
	TERM_lBrac,	
	TERM_rBrac,	

	// Relační operátory
	TERM_equal,	
	TERM_notEqual,	
	TERM_less,
	TERM_greater,		
	TERM_lessEqual,					
	TERM_greaterEqual,
 
	// i a $
	TERM_stackEnd,	
	TERM_variable,	

}	precTableIndex_t;

int expr_main();

int expr_algorithm();

precTableAction_t expr_readTable();

precTableIndex_t expr_getIndexFromChar();

char expr_getCharFromIndex();

int expr_shift();
int expr_reduce();

int expr_searchRule();
int expr_isAlgotihmFinished();  
int expr_generateResult();
int expr_convertTypes();
int expr_finishAlgorithm();

int expr_isFirstValid();

#endif