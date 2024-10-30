#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "stack.h"



int Parser(TokenList* list);
int LLGrammar(Stack* stack, Tokentype type);
int NonTerminalBodyPush(Stack* stack, Tokentype type);
int NonTerminalFunctionBodyPush(Stack* stack, Tokentype type);
int NonTerminalEolPush(Stack* stack, Tokentype type);
int NonTerminalParamsPush(Stack* stack, Tokentype type);
int NonTerminalNextParamsPush(Stack* stack, Tokentype type);
int NonTerminalBarsPush(Stack* stack, Tokentype type);
int NonTerminalElsePush(Stack* stack, Tokentype type);
int NonTerminalReturnTypePush(Stack* stack, Tokentype type);
int NonTerminalTypePush(Stack* stack, Tokentype type);
int NonTerminalVarOrConstPush(Stack* stack, Tokentype type);
int NonTerminalIdContinuePush(Stack* stack, Tokentype type);
int NonTerminalParamsEnterPush(Stack* stack, Tokentype type);
int NonTerminalNextParamsEnterPush(Stack* stack, Tokentype type);
int NonTerminalAfterBodyPush(Stack* stack, Tokentype type);
int NonTerminalTermPush(Stack* stack, Tokentype type);
//CHAT GPT HOVNO

void PrintStack(Stack* stack);

#endif