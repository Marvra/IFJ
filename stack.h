#ifndef STACK_H
#define STACK_H

#include "token.h"

typedef enum NonTerminal{
    NON_TERMINAL_UNKOWN,
    NON_T_BODY,
    NON_T_AFTER_BODY,
    NON_T_FUNCTION_BODY,
    NON_T_ID_CONTINUE,
    NON_T_PARAMS_ENTER,
    NON_T_NEXT_PARAMS_ENTER,
    NON_T_VAR_OR_CONST,
    NON_T_BARS,
    NON_T_ELSE,
    NON_T_PARAMS,
    NON_T_NEXT_PARAMS,
    NON_T_EOL,
    NON_T_RETURN_TYPE,
    NON_T_TYPE,
    NON_T_TERM,
} NonTerminal;

typedef struct StackItem{
    Tokentype tokenType;
    NonTerminal nonTerminal;

} StackItem;

typedef struct Stack{
    StackItem **items;
    int top;   
    int capacity;
} Stack;


Stack* InitStack();
StackItem* InitStackItem();
int Empty(Stack* stack);
void ResizeStack(Stack* stack);
void Push(Stack* stack, StackItem* item); 
void PushItem(Stack* stack, Tokentype type, NonTerminal nonTerminal); 
StackItem* Pop(Stack* stack);
StackItem* Top(Stack* stack);
void FreeStack(Stack* stack);
char* NonTerminalToString(NonTerminal nonTerminal);
void PrintStack(Stack* stack);

#endif