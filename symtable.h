/**
 * @file symtable.h
 * @author Robin Kurilla
 * @brief  header file for symbol table
 */

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
    TYPE_VOID
} NType;

typedef struct{
    bool isConstant;
    NType functionReturnType;
    int paramCount;
    NType *paramTypes;
    bool isUsed;                //var/const is used
    bool isKnown;               //value is known when compiling
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

//create/insert node
TNode* CreateNode(char *key);
TNode* InsertNode(TNode *rootPtr, char *key);

//rotations 
TNode* RotateRight(TNode *rootPtr);
TNode* RotateLeft(TNode *rootPtr);

//search node
TNode* SearchNode(TNode *rootPtr, char *key);
//get nodes
TNode* GetRightNode(TNode *rootPtr);
TNode* GetLeftNode(TNode *rootPtr);

//set functions
int SetType(TNode *rootPtr, char *key, NType type);
int SetFunctionReturnType(TNode *rootPtr, char *key, NType type);
int SetIsConstant(TNode *rootPtr, char *key, bool b);
int SetParameter(TNode *rootPtr, char *key, NType type);
int SetIsUsed(TNode *rootPtr, char *key);
int SetIsKnown(TNode *rootPtr, char *key);

//get functions
int GetType(TNode *rootPtr, char *key, NType *value);
int GetFunctionReturnType(TNode *rootPtr, char *key, NType *value);
int GetIsConstant(TNode *rootPtr, char *key, bool *value);
int GetParameters(TNode *rootPtr, char *key, NType *params);
int GetParameterCount(TNode *rootPtr, char *key, int *count);
bool GetIsUsed(TNode *node);
int GetIsKnown(TNode *rootPtr, char *key, bool *known);
int Height(TNode *rootPtr);

//free symtable
void FreeTree(TNode *rootPtr);

//maximum from a,b
int Max(int a, int b);

//function for SymList
SymList* CreateSymList();
void InsertTable(SymList *list, TNode *node);
SymListNode* GetLast(SymList *list);
SymListNode* GetNext(SymListNode *node);
TNode* GetTableNode(SymListNode *node);
void DeleteTable(SymList *list);
void FreeSymlist(SymList *list);

#endif