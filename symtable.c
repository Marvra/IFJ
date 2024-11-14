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
    newPtr->data.isNullable = false;
    newPtr->data.functionReturnType = TYPE_DEFAULT;
    newPtr->data.paramCount = 0;
    newPtr->data.paramTypes = NULL;
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

int GetIsNullable(TNode *rootPtr, char *key, bool *value){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        *value = rootPtr->data.isNullable;
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

int Height(TNode *rootPtr){
    if(rootPtr != NULL){ 
        return Max(Height(rootPtr->lPtr), Height(rootPtr->rPtr)) + 1;
    }else{
        return 0;
    }
}
