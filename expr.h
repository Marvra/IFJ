/**
 * @file expr.h
 * @author Vojtech Panek
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

#define PREC_TABLE_SIZE 16	
#define RULES_COUNT     14	
#define EXPR_ERROR      -1   
#define EXPR_SUCCESS    1  
#define EXPR_TRUE       1  
#define EXPR_FALSE      0  
#define TOK_FAIL        TOK_endOfFile	
#define TOK_BOOLEAN     100	
#define RESULT_ASSIGNMENT 'R'	
#define EXPR_RETURN_SUCC        0
#define EXPR_RETURN_ERROR_SYNTAX        2       
#define EXPR_RETURN_ERROR_SEM   3       
#define EXPR_RETURN_ERROR_TYPES   4       
#define EXPR_RETURN_ERROR_INTERNAL      99      
#define EXPR_RETURN_NOMORETOKENS 123    
#define EXPR_RETURN_STARTOVER 124       

typedef enum
{
	ACTION_shift,
	ACTION_reduce,
	ACTION_specialShift,
	ACTION_illegal	
}	precTableAction_t;

typedef enum
{
	TERM_plus,		
	TERM_minus,	
	TERM_divInt,	
	TERM_mul,		
	TERM_div,		
	TERM_lBrac,	
	TERM_rBrac,	
	TERM_stackEnd,	

	TERM_equal,	
	TERM_notEqual,	
	TERM_less,		
	TERM_lessEqual,	
	TERM_greater,	
	TERM_greaterEqual,	

        TERM_string,     

	TERM_expr,		
}	precTableIndex_t;

int expr_main();

int expr_algorithm();

precTableAction_t expr_readTable();

precTableIndex_t expr_getIndexFromChar();

char expr_getCharFromIndex();

int expr_shift();

int expr_reduce();
int expr_specialShift();
int expr_searchRule();
int expr_isAlgotihmFinished();  
int expr_generateResult();
int expr_convertTypes();
int expr_finishAlgorithm();

int expr_isFirstValid();

#endif