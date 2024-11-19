#include "symtable.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TNode* CreateNode(char *key){
    TNode *newPtr = (TNode *) malloc(sizeof(TNode));

    if(newPtr == NULL){
        fprintf(stderr, "SYMTABLE NODE: Memory allocation failed\n");
        exit(INTERNAL_ERROR);
    }

    newPtr->key = strdup(key);
    if(newPtr->key == NULL){
        fprintf(stderr, "SYMTABLE NODE: Memory allocation failed\n");
        exit(INTERNAL_ERROR);
    }
    newPtr->type = TYPE_DEFAULT;
    newPtr->data.isConstant = false;
    newPtr->data.functionReturnType = TYPE_DEFAULT;
    newPtr->data.paramCount = 0;
    newPtr->data.paramTypes = NULL;
    newPtr->data.isUsed = false;
    newPtr->data.isKnown = false;
    newPtr->lPtr = NULL;
    newPtr->rPtr = NULL;
    newPtr->height = 1;
    return newPtr;
}

TNode* InsertNode(TNode *rootPtr, char *key){
    if(rootPtr == NULL){
        return CreateNode(key);
    }

    int comparison = strcmp(rootPtr->key, key);
    if(comparison > 0){
        rootPtr->lPtr = InsertNode(rootPtr->lPtr, key);
    }else if(comparison < 0){
        rootPtr->rPtr = InsertNode(rootPtr->rPtr, key);
    }else{
        return rootPtr;
    }

    rootPtr->height = Max(Height(rootPtr->lPtr), Height(rootPtr->rPtr)) + 1;

    int balance = Height(rootPtr->lPtr) - Height(rootPtr->rPtr);

    if(balance > 1 && strcmp(key, rootPtr->lPtr->key) < 0){
        return RotateRight(rootPtr);
    }

    if(balance < -1 && strcmp(key, rootPtr->rPtr->key) > 0){
        return RotateLeft(rootPtr);
    }

    if(balance > 1 && strcmp(key, rootPtr->lPtr->key) > 0){
        rootPtr->lPtr = RotateLeft(rootPtr->lPtr);
        return RotateRight(rootPtr);
    }

    if(balance < -1 && strcmp(key, rootPtr->rPtr->key) < 0){
        rootPtr->rPtr = RotateRight(rootPtr->rPtr);
        return RotateLeft(rootPtr);
    }


    return rootPtr;
}

TNode* RotateRight(TNode *root){
    TNode *temp = root->lPtr;
    root->lPtr = temp->rPtr;
    temp->rPtr = root;

    root->height = Max(Height(root->lPtr), Height(root->rPtr)) + 1;
    temp->height = Max(Height(temp->lPtr), Height(temp->rPtr)) + 1;

    return temp;
}

TNode* RotateLeft(TNode *root){
    TNode *temp = root->rPtr;
    root->rPtr = temp->lPtr;
    temp->lPtr = root;

    root->height = Max(Height(root->lPtr), Height(root->rPtr)) + 1;
    temp->height = Max(Height(temp->lPtr), Height(temp->rPtr)) + 1;

    return temp;
}

TNode* SearchNode(TNode *rootPtr, char *key){
    if(rootPtr == NULL){
        return NULL;
    }

    int comparison = strcmp(rootPtr->key, key);
    if(comparison > 0){
        rootPtr = SearchNode(rootPtr->lPtr, key);
    }else if(comparison < 0){
        rootPtr = SearchNode(rootPtr->rPtr, key);
    }else{
        return rootPtr;
    }
    return rootPtr;
}

int SetType(TNode *rootPtr, char *key, NType type){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        temp->type = type;
        return 0;
    }
    return -1;
}

int SetFunctionReturnType(TNode *rootPtr, char *key, NType type){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        temp->data.functionReturnType = type;
        return 0;
    }
    return -1;
}

int SetIsConstant(TNode *rootPtr, char *key, bool b){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        temp->data.isConstant = b;
        return 0;
    }
    return -1;
}

int SetParameter(TNode *rootPtr, char *key, NType type){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        temp->data.paramTypes = (NType *)realloc(temp->data.paramTypes, (temp->data.paramCount + 1) * (sizeof(NType)));
        if(temp->data.paramTypes == NULL){
            fprintf(stderr, "NODE PARAMETERS: Memory allocation failed\n");
            exit(INTERNAL_ERROR);
        }
        temp->data.paramTypes[temp->data.paramCount] = type;
        temp->data.paramCount++;
        return 0;
    }
    return -1;
}

int SetIsUsed(TNode *rootPtr, char *key){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        temp->data.isUsed = true;
        return 0;
    }
    return -1;
}

int SetIsKnown(TNode *rootPtr, char *key){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        temp->data.isKnown = true;
        return 0;
    }
    return -1;
}

int GetType(TNode *rootPtr, char *key, NType *value){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        *value = temp->type;
        return 0;
    }
    return -1;
}

int GetFunctionReturnType(TNode *rootPtr, char *key, NType *value){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        *value = temp->data.functionReturnType;
        return 0;
    }
    return -1;
}

int GetIsConstant(TNode *rootPtr, char *key, bool *value){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        *value = temp->data.isConstant;
        return 0;
    }
    return -1;
}

int GetParameters(TNode *rootPtr, char *key, NType *params){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        for(int i = 0; i < temp->data.paramCount; i++){
            params[i] = temp->data.paramTypes[i];
        }
        return 0;
    }
    return -1;
}

int GetParameterCount(TNode *rootPtr, char *key, int *count){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        *count = temp->data.paramCount;
        return 0;
    }
    return -1;
}

int GetIsUsed(TNode *rootPtr, char *key, bool *used){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        *used = temp->data.isUsed;
        return 0;
    }
    return -1;
}

int GetIsKnown(TNode *rootPtr, char *key, bool *known){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        *known = temp->data.isKnown;
        return 0;
    }
    return -1;
}

void FreeTree(TNode *rootPtr){
    if(rootPtr != NULL){
        FreeTree(rootPtr->lPtr);
        FreeTree(rootPtr->rPtr);
        if(rootPtr->data.paramCount != 0){
            free(rootPtr->data.paramTypes);
        }
        free(rootPtr->key);
        free(rootPtr);
    }
}

int Max(int a, int b){
    if(a > b){
        return a;
    }else {
        return b;
    }
}

int Height(TNode* node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

SymList* CreateSymList(){
    SymList *list = (SymList*)malloc(sizeof(SymList));
    if(!list){
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    list->last = NULL;
    return list;
}

void InsertTable(SymList *list, TNode *node){
    SymListNode *newNode = (SymListNode*)malloc(sizeof(SymListNode));
    if(!newNode){
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    newNode->node = node;
    newNode->next = list->last;
    list->last = newNode;
}

SymListNode* GetLast(SymList *list){
    return list->last;
}

SymListNode* GetNext(SymListNode *node){
    return node->next;
}

TNode* GetTableNode(SymListNode *node){
    if(node == NULL){
        return NULL;
    }
    return node->node;
}

void DeleteTable(SymList *list){
    SymListNode *temp = list->last;
    list->last = temp->next;
    FreeTree(GetTableNode(temp));
    free(temp);
}
