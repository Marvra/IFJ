/**
 * @file parser.h
 * @author Martin Vrablec
 * @brief  header file for top down parser, function declarations for LL grammar checks and for correct building of AST
 * @todo
 */
#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "stack.h"
#include "ast.h"


// Functions for parser
int Parser(ASTNode** tree, TokenList* list);
int LLGrammar(Stack* stack, Tokentype type);
int NonTerminalBodyPush(Stack* stack, Tokentype type);
int NonTerminalFunctionBodyPush(Stack* stack, Tokentype type);
int NonTerminalParamsPush(Stack* stack, Tokentype type);
int NonTerminalNextParamsPush(Stack* stack, Tokentype type);
int NonTerminalBarsPush(Stack* stack, Tokentype type);
int NonTerminalReturnTypePush(Stack* stack, Tokentype type);
int NonTerminalTypePush(Stack* stack, Tokentype type);
int NonTerminalVarOrConstPush(Stack* stack, Tokentype type);
int NonTerminalIdHelper(Stack* stack, Tokentype type);
int NonTerminalIdContinuePush(Stack* stack, Tokentype type);
int NonTerminalDeclarationContinuePush(Stack* stack, Tokentype type);
int NonTerminalParamsEnterPush(Stack* stack, Tokentype type);
int NonTerminalNextParamsEnterPush(Stack* stack, Tokentype type);
int NonTerminalAfterBodyPush(Stack* stack, Tokentype type);
int NonTerminalTermPush(Stack* stack, Tokentype type);

// Functions for building AST
void BuildAST(ASTNode** expr_root, ASTNode** ast, Tokentype interestingToken, Token* token, Token* saveToken);
int InterestingTokens(Tokentype type);
DataType TokenTypeToDataType(Tokentype type);
ASTNode* findDeepestCodeNode(ASTNode** ast);
ASTNode* findDeepestFunDecNode(ASTNode** ast);
ASTNode* findDeepestParamNode(ASTNode** ast);
ASTNode* findDeepestFunctionBodyNode(ASTNode** ast);
ASTNode* findDeepestFuncCodeNode(ASTNode** ast);
ASTNode* findDeepestVarNode(ASTNode** ast);
ASTNode* findDeepestConstNode(ASTNode** ast);
ASTNode* findDeepestReturnNode(ASTNode** ast);
ASTNode* findDeepestAssignmentNode(ASTNode** ast);
ASTNode* findDeepestWhileNode(ASTNode** ast);
ASTNode* findDeepestWhileNodeHelp(ASTNode** ast);
ASTNode* findDeepestIfElseNode(ASTNode** ast);
ASTNode* findDeepestWhileNodeHelp(ASTNode** ast);
ASTNode* findFirstClosedIfNode(ASTNode** ast);
ASTNode* findFirstClosedIfNodeHelp(ASTNode** ast); 

#endif