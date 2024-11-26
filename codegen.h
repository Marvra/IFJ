#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void createHeader();
void createMain();
void createFunction(char *function_id);
void createFunctionCall(char* function_id);
int TraverseASTCodeGen(ASTNode *node);
void ExpressionsOperand(char* id, ASTNode* Input);
void ExpressionsOperatorInt(char* id, ASTNode* Input);
void ExpressionsOperatorFloat(char* id, ASTNode* Input);

#endif

