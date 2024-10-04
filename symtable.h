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
    int paramCount;
    NType *paramTypes;
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
int SetParameter(TNode *rootPtr, char *key, NType type);
int GetType(TNode *rootPtr, char *key, NType *value);
int GetFunctionReturnType(TNode *rootPtr, char *key, NType *value);
int GetIsNullable(TNode *rootPtr, char *key, bool *value);
int GetIsConstant(TNode *rootPtr, char *key, bool *value);
int GetParameters(TNode *rootPtr, char *key, NType *params);
int GetParameterCount(TNode *rootPtr, char *key, int *count);
void FreeTree(TNode *rootPtr);
int Max(int a, int b);
int Height(TNode *rootPtr);

#endif