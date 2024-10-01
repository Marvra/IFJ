#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>

typedef enum{
    TYPE_DEFAULT,
    TYPE_FUNCTION,
    TYPE_I32,
    TYPE_F64,
    TYPE_U8,
    TYPE_VOID
} NType;

typedef struct{
    bool isNullable;
    bool isConstant;
    NType functionReturnType;
    //PARAMETRE FUNKCIE
} NData;

typedef struct tnode{
    char *key;
    NType type;
    NData data;
    struct tnode *lPtr;
    struct tnode *rPtr;
} TNode;

TNode* CreateNode(char *key);
TNode* InsertNode(TNode *rootPtr, char *key);
TNode* RotateRight(TNode *rootPtr);
TNode* RotateLeft(TNode *rootPtr);
TNode* SearchNode(TNode *rootPtr, char *key);
int SetType(TNode *rootPtr, char *key, NType type);
int SetFunctionReturnType(TNode *rootPtr, char *key, NType type);
int SetIsNullable(TNode *rootPtr, char *key, bool b);
int SetIsConstant(TNode *rootPtr, char *key, bool b);
NType GetType(TNode *rootPtr, char *key);
NType GetFunctionReturnType(TNode *rootPtr, char *key);
bool IsNullable(TNode *rootPtr, char *key);
bool IsConstant(TNode *rootPtr, char *key);
void FreeTree(TNode *rootPtr);
int Max(int a, int b);
int Height(TNode *rootPtr);

#endif