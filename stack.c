#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "error.h"
#include "stack.h"


Stack* InitStack()
{
    Stack* stack = (Stack*) malloc(sizeof(Stack));
    stack->capacity = 1;
    stack->top = -1;
    stack->items = (StackItem**) malloc(sizeof(StackItem*) * stack->capacity);
    return stack;
}

StackItem* InitStackItem() {
    StackItem* item = (StackItem*) malloc(sizeof(StackItem)); 
    item->nonTerminal = NON_TERMINAL_UNKOWN;
    item->tokenType = TOKEN_UNKNOWN;
    return item;
}

int Empty(Stack* stack) {
    return stack->top == -1;
}

void ResizeStack(Stack* stack) 
{
    stack->capacity *= 2;
    stack->items = (StackItem**) realloc(stack->items, sizeof(StackItem*) * stack->capacity);

    if (stack->items == NULL)
    {
        printf("Error: Memory allocation failed while resizing the stack.\n");
        exit(EXIT_FAILURE);
    }
}

void Push(Stack* stack, StackItem* item) 
{
    if (stack->top == stack->capacity - 1) 
    {
        ResizeStack(stack);
    }
    stack->top++;
    stack->items[stack->top] = item; 
}

void PushItem(Stack* stack, Tokentype type, NonTerminal nonTerminal) 
{
    StackItem* item = InitStackItem();
    item->tokenType = type;
    item->nonTerminal = nonTerminal;
    Push(stack, item);
}


StackItem* Pop(Stack* stack)
{
    if (Empty(stack)) 
    {
        printf("Error: Stack underflow\n");
        return NULL;
    }
    return stack->items[stack->top--];
}

StackItem* Top(Stack* stack) 
{
    if (Empty(stack)) 
    {
        printf("Error: Stack is empty\n");
        return NULL;
    }
    return stack->items[stack->top];
}

void FreeStack(Stack* stack) 
{
    free(stack->items);
    free(stack);
}

char* NonTerminalToString(NonTerminal nonTerminal) 
{
    switch (nonTerminal) 
    {
        case NON_TERMINAL_UNKOWN: return "NON_TERMINAL_UNKOWN";
        case NON_T_BODY: return "NON_T_BODY";
        case NON_T_FUNCTION_BODY: return "NON_T_FUNCTION_BODY";
        case NON_T_EOL: return "NON_T_EOL";
        case NON_T_PARAMS: return "NON_T_PARAMS";
        case NON_T_NEXT_PARAMS: return "NON_T_NEXT_PARAMS";
        case NON_T_BARS: return "NON_T_BARS";
        case NON_T_TYPE: return "NON_T_TYPE";
        case NON_T_RETURN_TYPE: return "NON_T_RETURN_TYPE";
        case NON_T_VAR_OR_CONST: return "NON_T_VAR_OR_CONST";
        case NON_T_ID_CONTINUE: return "NON_T_ID_CONTINUE";
        case NON_T_PARAMS_ENTER: return "NON_T_PARAMS_ENTER";
        case NON_T_NEXT_PARAMS_ENTER: return "NON_T_NEXT_PARAMS_ENTER";
        case NON_T_AFTER_BODY: return "NON_T_AFTER_BODY";
        case NON_T_TERM: return "NON_T_TERM";
        default: return "UNKNOWN_NON_TERMINAL";
    }
}

void PrintStack(Stack* stack)
{
    if (Empty(stack)) {
        printf("Stack is empty.\n");
        return;
    }

    printf("\nSTART STACK ------ (from top to bottom):\n");

    for (int i = stack->top; i >= 0; i--) {

        StackItem* item = stack->items[i];
        
        if (item->nonTerminal != NON_TERMINAL_UNKOWN) {
            printf("Non-terminal: %s\n", NonTerminalToString(item->nonTerminal));
        } 
        else if (item->tokenType != TOKEN_UNKNOWN) {
            printf("Token: %s\n", TokenTypeString(item->tokenType));
        }
    }

    printf("END STACK ------ (from top to bottom)\n\n");
}