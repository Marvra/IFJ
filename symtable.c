#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TNode* CreateNode(char *key){
    TNode *newPtr = (TNode *) malloc(sizeof(TNode));

    if(newPtr == NULL){
        fprintf(stderr, "TREE NODE: Memory allocation failed\n");
        exit(1);
    }

    newPtr->key = strdup(key);
    newPtr->type = TYPE_DEFAULT;
    newPtr->data.isConstant = false;
    newPtr->data.isNullable = false;
    newPtr->data.functionReturnType = TYPE_DEFAULT;
    newPtr->lPtr = NULL;
    newPtr->rPtr = NULL;
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

    int balance = Height(rootPtr->lPtr) - Height(rootPtr->rPtr);
    if(balance > 1){
        if(comparison > 0){
            return RotateRight(rootPtr);
        }else{
            rootPtr->lPtr = RotateLeft(rootPtr->lPtr);
            return RotateRight(rootPtr);
        }
    }
    if(balance < -1){
        if(comparison > 0){
            rootPtr->rPtr = RotateRight(rootPtr->rPtr);
            return RotateLeft(rootPtr);
        }else{
            return RotateLeft(rootPtr);
        }
    }

    return rootPtr;
}

TNode* RotateRight(TNode *rootPtr){
    TNode *temp = rootPtr->lPtr;
    rootPtr->lPtr = temp->rPtr;
    temp->rPtr = rootPtr;
    return temp;
}

TNode* RotateLeft(TNode *rootPtr){
    TNode *temp = rootPtr->rPtr;
    rootPtr->rPtr = temp->lPtr;
    temp->lPtr = rootPtr;
    return temp;
}

TNode* SearchNode(TNode *rootPtr, char *key){
    if(rootPtr == NULL){
        return NULL;
    }

    int comparison = strcmp(rootPtr->key, key);
    if(comparison > 0){
        rootPtr->lPtr = SearchNode(rootPtr->lPtr, key);
    }else if(comparison < 0){
        rootPtr->rPtr = SearchNode(rootPtr->rPtr, key);
    }else{
        return rootPtr;
    }
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

int SetIsNullable(TNode *rootPtr, char *key, bool b){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        temp->data.isNullable = b;
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


NType GetType(TNode *rootPtr, char *key){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        return rootPtr->type;
    }
    return TYPE_DEFAULT;
}

NType GetFunctionReturnType(TNode *rootPtr, char *key){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        return rootPtr->data.functionReturnType;
    }
    return TYPE_DEFAULT;
}

//Lepsie by bolo predavat hodnotu cez argument a funkciou vraciat ci bola Node najdena alebo nie
bool IsNullable(TNode *rootPtr, char *key){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        return rootPtr->data.isNullable;
    }
    return false;
}

//Lepsie by bolo predavat hodnotu cez argument a funkciou vraciat ci bola Node najdena alebo nie
bool IsConstant(TNode *rootPtr, char *key){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        return rootPtr->data.isConstant;
    }
    return false;
}

void FreeTree(TNode *rootPtr){
    if(rootPtr != NULL){
        FreeTree(rootPtr->lPtr);
        FreeTree(rootPtr->rPtr);
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

int Height(TNode *rootPtr){
    if(rootPtr != NULL){ 
        return Max(Height(rootPtr->lPtr), Height(rootPtr->rPtr)) + 1;
    }else{
        return 0;
    }
}
