/**
 * @file codegen.h
 * @author Robin Kurilla, Jaroslav Doktor
 * @brief  header file for generarting code
 * @todo
 */

#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantic.h"

void createHeader();
void createMain();
void createFunction(char *function_id);
void createVariable(char* var_id);
void CreateNonVariableParams(int param);
char* concat(const char *s1, const char *s2);
char* removeFirstAndLast(const char* str);
char* WriteString(ASTNode* node);
void CreateArguments(ASTNode *node);
void CreateParams(ASTNode *node);
void createFunctionCall(char* function_id);
void CreateExpression(ASTNode *node);
void CreateReturn(ASTNode *node);
void RelOperator(ASTNode *node, int cond);
void RelOperatorWhile(ASTNode *node, int cond);
void StartIfElse(ASTNode *node, int cond);
void CreateWhile(ASTNode *node, int cond);
void TraverseASTCodeGen(ASTNode *node);
void CodeGen(ASTNode *node);

#endif

