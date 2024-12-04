/**
 * Project: IFJ24 2024
 * Robin Kurilla (xkuril03)
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symtable.h"

// Sémantická analýza, ak nastane error vracia chybový kód inak 0
int SemanticAnalysis(ASTNode *root);

#endif