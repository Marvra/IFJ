/**
 * @file stack.h
 * @author Martin Vrablec
 * @brief  header file for structure of stack used in top down parser, function declarations for stack operations, non terminal types ,stack hold info about non terminals and tokens
 * @todo
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "error.h"
#include "stack.h"

/**
 * @brief initializing stack
 * @return pointer to stack
 */
Stack* InitStack()
{
    Stack* stack = (Stack*) malloc(sizeof(Stack));
    stack->capacity = 1;
    stack->top = -1;
    stack->items = (StackItem**) malloc(sizeof(StackItem*) * stack->capacity);
    return stack;
}

/**
 * @brief initializing stack item
 * @return pointer to stack item
 */
StackItem* InitStackItem() {
    StackItem* item = (StackItem*) malloc(sizeof(StackItem)); 
    item->nonTerminal = NON_TERMINAL_UNKOWN;
    item->tokenType = TOKEN_UNKNOWN;
    return item;
}

/**
 * @brief checking if stack is empty
 * @param stack pointer to stack
 * @return 1 if stack is empty, 0 if stack is not empty
 */
int Empty(Stack* stack) {
    return stack->top == -1;
}

/**
 * @brief resizing stack
 * @param stack pointer to stack
 */
void ResizeStack(Stack* stack) 
{
    stack->capacity *= 2;
    stack->items = (StackItem**) realloc(stack->items, sizeof(StackItem*) * stack->capacity);

    if (stack->items == NULL)
    {
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief pushing item to stack
 * @param stack pointer to stack
 * @param item pointer to stack item
 */
void Push(Stack* stack, StackItem* item) 
{
    // stack is full resizing it
    if (stack->top == stack->capacity - 1) 
    {
        ResizeStack(stack);
    }
    stack->top++;
    stack->items[stack->top] = item; 
}

/**
 * @brief pushing item to stack
 * @param stack pointer to stack
 * @param type type of token
 * @param nonTerminal non terminal type
 */
void PushItem(Stack* stack, Tokentype type, NonTerminal nonTerminal) 
{
    StackItem* item = InitStackItem();
    item->tokenType = type;
    item->nonTerminal = nonTerminal;
    Push(stack, item);
}

/**
 * @brief popping item from stack
 * @param stack pointer to stack
 * @return pointer to stack item. if stack is empty return NULL
 */
StackItem* Pop(Stack* stack)
{
    if (Empty(stack)) 
    {
        return NULL;
    }
    return stack->items[stack->top--];
}

/**
 * @brief getting top item from stack
 * @param stack pointer to stack
 * @return pointer to stack item. if stack is empty return NULL
 */
StackItem* Top(Stack* stack) 
{
    if (Empty(stack)) 
    {
        return NULL;
    }
    return stack->items[stack->top];
}

/**
 * @brief freeing stack
 * @param stack pointer to stack
 */
void FreeStack(Stack* stack) 
{
    free(stack->items);
    free(stack);
}