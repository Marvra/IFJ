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
ASTNode* findDeepestCodeNode(ASTNode** ast);
ASTNode* findDeepestFunDecNode(ASTNode** ast);
ASTNode* findDeepestParamNode(ASTNode** ast);
ASTNode* findDeepestFunctionBodyNode(ASTNode** ast);
ASTNode* findDeepestFuncCodeNode(ASTNode** ast);
ASTNode* findDeepestVarNode(ASTNode** ast);
ASTNode* findDeepestConstNode(ASTNode** ast);
ASTNode* findDeepestWhileNode(ASTNode** ast);
ASTNode* findDeepestWhileNodeHelp(ASTNode** ast);
ASTNode* findFirstClosedIfNode(ASTNode** ast);
ASTNode* findFirstClosedIfNodeHelp(ASTNode** ast); 
//CHAT GPT HOVNO
void exportASTToDot(ASTNode* root);
void printDotAST(ASTNode* node, FILE* file);

void PrintStack(Stack* stack);

#endif