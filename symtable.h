/**
 * Project: IFJ24 2024
 * Robin Kurilla (xkuril03)
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

//Pomocná funkcia na vytváranie nody
TNode* CreateNode(char *key);
//Vkladá(vytvára) nodu v tabuľke symbolov
TNode* InsertNode(TNode *rootPtr, char *key);
//Rotácia vpravo
TNode* RotateRight(TNode *rootPtr);
//Rotácia vľavo
TNode* RotateLeft(TNode *rootPtr);
//Vracia najdenú nodu aleno null
TNode* SearchNode(TNode *rootPtr, char *key);
//Vracia pravú nodu
TNode* GetRightNode(TNode *rootPtr);
//Vracia ľavú nodu
TNode* GetLeftNode(TNode *rootPtr);

//Nastavuje typ nody
int SetType(TNode *rootPtr, char *key, NType type);
//Nastavuje návratový typ funkcie
int SetFunctionReturnType(TNode *rootPtr, char *key, NType type);
//Nastavuje premennú na konštantu
int SetIsConstant(TNode *rootPtr, char *key, bool b);
//Pridáva parameter funkcii
int SetParameter(TNode *rootPtr, char *key, NType type);
//Nastavuje že bola použitá
int SetIsUsed(TNode *rootPtr, char *key);
//Nastavuje že je známa pri preklade
int SetIsKnown(TNode *rootPtr, char *key);

//Vracia typ nody
int GetType(TNode *rootPtr, char *key, NType *value);
//Vracia návratový typ funkcie
int GetFunctionReturnType(TNode *rootPtr, char *key, NType *value);
//Vracia či je to konštanta
int GetIsConstant(TNode *rootPtr, char *key, bool *value);
//Vracia parametre
int GetParameters(TNode *rootPtr, char *key, NType *params);
//Vracia počet parametrov
int GetParameterCount(TNode *rootPtr, char *key, int *count);
//Vracia či bola použitá
bool GetIsUsed(TNode *node);
//Vracia či je známa pri preklade
int GetIsKnown(TNode *rootPtr, char *key, bool *known);

//Uvoľnuje pamäť
void FreeTree(TNode *rootPtr);
//Maximum z a,b
int Max(int a, int b);
//Vracia výšku uzlu
int Height(TNode *rootPtr);

//Vytvára zoznam tabuliek symbolov
SymList* CreateSymList();
//Vkladá tabuľku symbolov do zoznamu
void InsertTable(SymList *list, TNode *node);
//Vracia posledný prvok zoznamu
SymListNode* GetLast(SymList *list);
//Vracia nasledujúcu prvok zoznamu
SymListNode* GetNext(SymListNode *node);
//Získava tabuľku symbolov z prvku zoznamu
TNode* GetTableNode(SymListNode *node);
//Uvoľnuje posledny prvok spolu s jeho tabuľkou symbolov
void DeleteTable(SymList *list);
//Uvoľnuje cely zoznam
void FreeSymlist(SymList *list);

#endif