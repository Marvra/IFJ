/**
 * Project: IFJ24 2024
 * Robin Kurilla (xkuril03)
 */

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
void TraverseASTCodeGen(ASTNode *node);
void CodeGen(ASTNode *node);
char* WriteString(ASTNode* node);

#endif

