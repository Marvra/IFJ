#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "stack.h"
#include "ast.h"



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
// building ast 
void BuildAST(ASTNode** ast, Tokentype interestingToken, Token* token);
int InterestingTokens(Tokentype type);
DataType TokenTypeToDataType(Tokentype type);
// Forward declarations
ASTNode* findDeepestCodeNode(ASTNode** ast);
ASTNode* findDeepestFunDecNode(ASTNode** ast);
ASTNode* findDeepestParamNode(ASTNode** ast);
ASTNode* findDeepestFunctionBodyNode(ASTNode** ast);
ASTNode* findDeepestVarNode(ASTNode** ast);
ASTNode* findDeepestConstNode(ASTNode** ast);
// void ProcessToken(ASTNode* ast, ASTNode** funcDec, ASTNode** funcBodyDec, TokenList* list, NonTerminal lastNonTerminal, Tokentype* lastInterestingToken);
// void HandlePublicFunction(ASTNode* ast, TokenList* list, ASTNode** funcDec);
// void HandleFunctionDeclaration(ASTNode** funcDec, ASTNode** funcBodyDec, TokenList* list);
// void HandleVariableDeclaration(ASTNode** funcBodyDec, TokenList* list);
// void HandleIdentifier(ASTNode** funcDec, TokenList* list);
// void HandleFunctionParameter(ASTNode** funcDec, TokenList* list);
// void HandleType(ASTNode** funcDec, DataType type);
//CHAT GPT HOVNO

void PrintStack(Stack* stack);

#endif