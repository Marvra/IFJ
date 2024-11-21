#ifndef LINKED_LIST_H
#define LINKED_LIST_H


#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "token.h"
#include "parser.h"

typedef enum precTableTerm_t {
	TERM_plusMinus,
	TERM_mulDiv,
	TERM_leftBracket,
	TERM_rightBracket,
	TERM_relational,
	TERM_variable,
	TERM_stackEnd,
	NO_TERM_E, // E 
} precTableTerm_t;


typedef enum precTableAction_t {
	ACTION_reduce, // <
	ACTION_shift, // >
	ACTION_equal, // =
	ACTION_error, // #
	ACTION_end, // @
} precTableAction_t;
typedef enum {
	RULE_plusMinus, // E -> E + E || E - E
	RULE_mulDiv, // E -> E * E || E / E
	RULE_brackets, // E -> (E)
	RULE_relational, // E -> E == E || E != E || E >= ...
	RULE_e, // E -> i
} precTableRule_t;

typedef struct {
    precTableTerm_t termType;
} *DLLData;

typedef struct DLLElement {
	DLLData data;
	struct DLLElement *previousElement;
	struct DLLElement *nextElement;
} *DLLElementPtr;

typedef struct {
	DLLElementPtr firstElement;
	DLLElementPtr lastElement;
	int currentLength;
} DLList;

DLList* DLLInit();

void DLLDispose(DLList*);

int DLLInsertLast(DLList*, precTableTerm_t);

DLLData DLLDeleteLast(DLList *);

void DLLPrintTerms(DLList *);

#endif