/**
 * @file stack.h
 * @author Martin Vrablec
 * @brief  header file for structure of stack used in top down parser, function declarations for stack operations, non terminal types ,stack holds info about non terminals and tokens
 * @todo
 */
#ifndef STACK_H
#define STACK_H

#include "token.h"

typedef enum NonTerminal{
    NON_TERMINAL_UNKOWN,
    NON_T_BODY,
    NON_T_AFTER_BODY,
    NON_T_FUNCTION_BODY,
    NON_T_ID_HELPER,
    NON_T_ID_CONTINUE,
    NON_T_DECLARATION_CONTINUE,
    NON_T_PARAMS_ENTER,
    NON_T_NEXT_PARAMS_ENTER,
    NON_T_VAR_OR_CONST,
    NON_T_BARS,
    NON_T_PARAMS,
    NON_T_NEXT_PARAMS,
    NON_T_EOL,
    NON_T_RETURN_TYPE,
    NON_T_TYPE,
    NON_T_TERM,
    NON_T_EXPR
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
StackItem* Pop(Stack* stack);
StackItem* Top(Stack* stack);
int Empty(Stack* stack);
void ResizeStack(Stack* stack);
void Push(Stack* stack, StackItem* item); 
void PushItem(Stack* stack, Tokentype type, NonTerminal nonTerminal); 
void FreeStack(Stack* stack);

#endif