#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>

typedef enum{
    TYPE_DEFAULT,
    TYPE_FUNCTION,
    TYPE_I32,
    TYPE_F64,
    TYPE_U8,
    TYPE_I32_N,
    TYPE_F64_N,
    TYPE_U8_N,
    TYPE_PARAMS,
    TYPE_VOID
} NType;

typedef struct{
    bool isConstant;
    NType functionReturnType;
    int paramCount;
    NType *paramTypes;
    bool isUsed;
    bool isKnown;
} NData;

typedef struct tnode{
    char *key;
    NType type;
    NData data;
    struct tnode *lPtr;
    struct tnode *rPtr;
    int height;
} TNode;

typedef struct SymListNode {
    TNode *node;
    struct SymListNode *next;
} SymListNode;

typedef struct {
    SymListNode *last;
} SymList;

TNode* CreateNode(char *key);
TNode* InsertNode(TNode *rootPtr, char *key);
TNode* RotateRight(TNode *rootPtr);
TNode* RotateLeft(TNode *rootPtr);
TNode* SearchNode(TNode *rootPtr, char *key);
int SetType(TNode *rootPtr, char *key, NType type);
int SetFunctionReturnType(TNode *rootPtr, char *key, NType type);
int SetIsConstant(TNode *rootPtr, char *key, bool b);
int SetParameter(TNode *rootPtr, char *key, NType type);
int SetIsUsed(TNode *rootPtr, char *key);
int SetIsKnown(TNode *rootPtr, char *key);
int GetType(TNode *rootPtr, char *key, NType *value);
int GetFunctionReturnType(TNode *rootPtr, char *key, NType *value);
int GetIsConstant(TNode *rootPtr, char *key, bool *value);
int GetParameters(TNode *rootPtr, char *key, NType *params);
int GetParameterCount(TNode *rootPtr, char *key, int *count);
bool GetIsUsed(TNode *node);
int GetIsKnown(TNode *rootPtr, char *key, bool *known);
bool GetIsUsed(TNode *node);
TNode* GetRightNode(TNode *rootPtr);
TNode* GetLeftNode(TNode *rootPtr);
void FreeTree(TNode *rootPtr);
int Max(int a, int b);
int Height(TNode *rootPtr);

//-------------------
SymList* CreateSymList();
void InsertTable(SymList *list, TNode *node);
SymListNode* GetLast(SymList *list);
SymListNode* GetNext(SymListNode *node);
TNode* GetTableNode(SymListNode *node);
void DeleteTable(SymList *list);

#endif