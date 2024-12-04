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
void createFunctionCall(char* function_id);
void TraverseASTCodeGen(ASTNode *node);
void CodeGen(ASTNode *node);
char* WriteString(ASTNode* node);

#endif

