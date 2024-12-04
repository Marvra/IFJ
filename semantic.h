/**
 * @file semantic.h
 * @author Robin Kurilla
 * @brief  header file for semantic analysis
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symtable.h"

//Conversion functions
NType DataTypeToNType(DataType type);
DataType NTypeToDataType(NType type);

void FreeSemantics(ASTStack *stack, SymList *list);
void AddBuiltInFunctionsToSymtable(TNode **symtable);
int LookForReturnNodes(ASTNode *node);
int AnalyzeFunDec(ASTNode *node, TNode **symtable);
int CheckForMainFunction(TNode **symtable);
TNode* GetFunctionSymtable(ASTNode *node, DataType *retType);
TNode* FindInSymlist(SymList *list, char *id);
TNode* GetBlockSymtable(ASTNode *node, SymList *list, int *error);
int AnalyzeExpression(ASTNode *node, SymList *list, DataType *expType, bool *isKnown);
int AnalyzeFunctionCall(ASTNode *node, SymList *list, NType *type);
int AnalyzeVariableDeclaration(ASTNode *node, SymList *list);
int AnalyzeAssignment(ASTNode *node, SymList *list);
int AnalyzeCondition(ASTNode *node, SymList *list, bool *hasNullId);
int AnalyzeReturnNode(ASTNode *node, SymList *list, DataType returnType);
int CheckVariablesUsed(TNode *node);
int SemanticAnalysis(ASTNode *root);

#endif