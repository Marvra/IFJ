/**
 * @file symtable.c
 * @author Robin Kurilla
 * @brief  source file for symbol table
 */

#include "symtable.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * @brief creates node with key
 * @param key identificator
 * @return created node
 */
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

/**
 * @brief inserts node in the right place into symtable
 * @param rootPtr symtable root
 * @param key identificator
 * @return symtable root node
 */
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

    // sets new height for node
    rootPtr->height = Max(Height(rootPtr->lPtr), Height(rootPtr->rPtr)) + 1;

    // balance factor
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

/**
 * @brief rotation right and new heights
 * @param root node that will be rotated
 * @return new top node
 */
TNode* RotateRight(TNode *root){
    TNode *temp = root->lPtr;
    root->lPtr = temp->rPtr;
    temp->rPtr = root;

    root->height = Max(Height(root->lPtr), Height(root->rPtr)) + 1;
    temp->height = Max(Height(temp->lPtr), Height(temp->rPtr)) + 1;

    return temp;
}

/**
 * @brief rotation left and new heights
 * @param root node that will be rotated
 * @return new top node
 */
TNode* RotateLeft(TNode *root){
    TNode *temp = root->rPtr;
    root->rPtr = temp->lPtr;
    temp->lPtr = root;

    root->height = Max(Height(root->lPtr), Height(root->rPtr)) + 1;
    temp->height = Max(Height(temp->lPtr), Height(temp->rPtr)) + 1;

    return temp;
}

/**
 * @brief searches for node in symtable
 * @param rootPtr symtable root
 * @param key identificator
 * @return NULL or found node
 */
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

/**
 * @brief sets node type for id node
 * @param rootPtr symtable node
 * @param key identificator
 * @param type type to be set
 * @return 0 or when error -1
 */
int SetType(TNode *rootPtr, char *key, NType type){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        temp->type = type;
        return 0;
    }
    return -1;
}

/**
 * @brief sets return type for function nodes for id node
 * @param rootPtr symtable node
 * @param key identificator
 * @param type type to be set
 * @return 0 or when error -1
 */
int SetFunctionReturnType(TNode *rootPtr, char *key, NType type){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        temp->data.functionReturnType = type;
        return 0;
    }
    return -1;
}

/**
 * @brief sets if node is constant for id node
 * @param rootPtr symtable node
 * @param key identificator
 * @param b true / false - is constant 
 * @return 0 or when error -1
 */
int SetIsConstant(TNode *rootPtr, char *key, bool b){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        temp->data.isConstant = b;
        return 0;
    }
    return -1;
}

/**
 * @brief sets function parameter for id node
 * @param rootPtr symtable node
 * @param key identificator
 * @param type parameter type
 * @return 0 or when error -1
 */
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

/**
 * @brief set that variable is used for id node
 * @param rootPtr symtable node
 * @param key identificator
 * @return 0 or when error -1
 */
int SetIsUsed(TNode *rootPtr, char *key){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        temp->data.isUsed = true;
        return 0;
    }
    return -1;
}

/**
 * @brief set that variable is known when compiling for id node
 * @param rootPtr symtable node
 * @param key identificator
 * @return 0 or when error -1
 */
int SetIsKnown(TNode *rootPtr, char *key){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        temp->data.isKnown = true;
        return 0;
    }
    return -1;
}

/**
 * @brief gets node type from id node
 * @param rootPtr symtable node
 * @param key identificator
 * @param value returns node type
 * @return 0 or when error -1
 */
int GetType(TNode *rootPtr, char *key, NType *value){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        *value = temp->type;
        return 0;
    }
    return -1;
}

/**
 * @brief gets function return type from id node
 * @param rootPtr symtable node
 * @param key identificator
 * @param value returns function return type
 * @return 0 or when error -1
 */
int GetFunctionReturnType(TNode *rootPtr, char *key, NType *value){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        *value = temp->data.functionReturnType;
        return 0;
    }
    return -1;
}

/**
 * @brief gets if variable is constant from id node
 * @param rootPtr symtable node
 * @param key identificator
 * @param value returns if is constant
 * @return 0 or when error -1
 */
int GetIsConstant(TNode *rootPtr, char *key, bool *value){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        *value = temp->data.isConstant;
        return 0;
    }
    return -1;
}

/**
 * @brief gets function parameters from id node
 * @param rootPtr symtable node
 * @param key identificator
 * @param params returns parameter types
 * @return 0 or when error -1
 */
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

/**
 * @brief gets function parameter count from id node
 * @param rootPtr symtable node
 * @param key identificator
 * @param count returns parameter count
 * @return 0 or when error -1
 */
int GetParameterCount(TNode *rootPtr, char *key, int *count){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        *count = temp->data.paramCount;
        return 0;
    }
    return -1;
}

/**
 * @brief gets if variable was used
 * @param node node from which we get 
 * @return true if its used
 */
bool GetIsUsed(TNode *node){
    return node->data.isUsed;
}

/**
 * @brief gets if variable is known from id node
 * @param rootPtr symtable node
 * @param key identificator
 * @param known returns if is known
 * @return 0 or when error -1
 */
int GetIsKnown(TNode *rootPtr, char *key, bool *known){
    TNode *temp = SearchNode(rootPtr, key);
    if(temp != NULL){
        *known = temp->data.isKnown;
        return 0;
    }
    return -1;
}

/**
 * @brief gets right node
 * @param rootPtr symtable node
 * @return right node from rootPtr
 */
TNode* GetRightNode(TNode *rootPtr){
    if(rootPtr == NULL){
        return NULL;
    }
    return rootPtr->rPtr;
}

/**
 * @brief gets left node
 * @param rootPtr symtable node
 * @return right node from rootPtr
 */
TNode* GetLeftNode(TNode *rootPtr){
    if(rootPtr == NULL){
        return NULL;
    }
    return rootPtr->lPtr;
}

/**
 * @brief frees symtable
 * @param rootPtr symtable root
 */
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

/**
 * @brief max from 2 numbers
 * @param a value1
 * @param b value2
 * @return higher number
 */
int Max(int a, int b){
    if(a > b){
        return a;
    }else {
        return b;
    }
}

/**
 * @brief gets height of node
 * @param rootPtr symtable node
 * @return node height
 */
int Height(TNode* node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

/**
 * @brief creates SymList
 * @return created SymList
 */
SymList* CreateSymList(){
    SymList *list = (SymList*)malloc(sizeof(SymList));
    if(!list){
        fprintf(stderr, "Memory allocation failed\n");
        exit(INTERNAL_ERROR);
    }
    list->last = NULL;
    return list;
}

/**
 * @brief inserts new symtable into list to the end
 * @param list SymList
 * @param node symtable
 */
void InsertTable(SymList *list, TNode *node){
    SymListNode *newNode = (SymListNode*)malloc(sizeof(SymListNode));
    if(!newNode){
        fprintf(stderr, "Memory allocation failed\n");
        exit(INTERNAL_ERROR);
    }
    newNode->node = node;
    newNode->next = list->last;
    list->last = newNode;
}

/**
 * @brief gets last SymList node
 * @param list SymList
 * @return last SymList node
 */
SymListNode* GetLast(SymList *list){
    return list->last;
}

/**
 * @brief gets next SymList node
 * @param node SymList node
 * @return next SymList node from node
 */
SymListNode* GetNext(SymListNode *node){
    return node->next;
}

/**
 * @brief gets symtable from SymList node
 * @param node SymList node
 * @return symtable from node
 */
TNode* GetTableNode(SymListNode *node){
    if(node == NULL){
        return NULL;
    }
    return node->node;
}

/**
 * @brief deletes last SymList node with its symtable
 * @param list SymList
 */
void DeleteTable(SymList *list){
    SymListNode *temp = list->last;
    list->last = temp->next;
    FreeTree(GetTableNode(temp));
    free(temp);
}

/**
 * @brief frees SymList
 * @param list SymList
 */
void FreeSymlist(SymList *list){
    if(list == NULL){
        return;
    }

    while(list->last != NULL){
        DeleteTable(list);
    }

    free(list);
}