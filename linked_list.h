#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "token.h"
#include "expr.h"

typedef struct {
    precTableTerm_t termType; // Opravit nefunguje
    NonTerminal nonTerminal;
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

void DLLInsertLast(DLList*, precTableTerm_t, NonTerminal);

void DLLDeleteLast(DLList *);