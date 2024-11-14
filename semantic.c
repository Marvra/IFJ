#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*  ZISKANIE PARAMETROV ZO SYMTABLE
    int count;
    GetParameterCount(*symtable, idFunction, &count);
    NType pt[count];
    GetParameters(*symtable, idFunction, pt);
*/

NType DataTypeToNType(DataType type){
    switch(type){
        case T_F64:
            return TYPE_F64;
        case T_VOID:
            return TYPE_VOID;
        case T_I32:
            return TYPE_I32;
        case T_U8:
            return TYPE_U8;
        default:
            return TYPE_DEFAULT;
    }
}

int AnalyzeFunDec(ASTNode *node, TNode **symtable){
    if(node == NULL || node->type != TYPE_FUN_DECL){
        //NO ID, return error code
        return 0;
    }
    ASTNode *temp = GetIdNode(node);
    char *idFunction = GetId(temp);
    printf("function with id: %s\n", idFunction);
    *symtable = InsertNode(*symtable,idFunction);
    SetType(*symtable, idFunction, TYPE_FUNCTION);
    //Create function in symtable
    DataType type = GetDataType(temp);
    SetFunctionReturnType(*symtable, idFunction, DataTypeToNType(type));
    //add return type to table item
    temp = GetParamNode(temp);
    while(temp != NULL){
        char *id = GetId(temp);
        printf("param with id: %s\n", id);
        //add to local symtable -> for code block

        type = GetDataType(temp);
        SetParameter(*symtable, idFunction, DataTypeToNType(type));

        temp = GetParamNode(temp);
    }
    //taktiez by bolo asi fajn pozriet ci je spravne return v bloku funkcie
    return 0;

}

int CheckForMainFunction(TNode **symtable){
    NType type;
    int error = GetType(*symtable, "main", &type);
    if(error == -1 || type != TYPE_FUNCTION){
        //error main chyba
        return -1;
    }
    int count;
    error = GetParameterCount(*symtable, "main", &count);
    if(error == -1 || count != 0){
        //error main chyba
        return -1;
    }
    error = GetFunctionReturnType(*symtable, "main", &type);
    if(error == -1 || type != TYPE_VOID){
        //error main chyba
        return -1;
    }
    return 0;
}

TNode* GetFunctionSymtable(ASTNode *node){
    TNode *newSymtable = NULL;
    ASTNode *temp = GetIdNode(node);
    temp = GetParamNode(temp);
    while(temp != NULL){
        char *id = GetId(temp);
        newSymtable = InsertNode(newSymtable, id);
        DataType type = GetDataType(temp);
        SetType(newSymtable, id, DataTypeToNType(type));
        //MOZNO TO ESTE MA BYT CONSTANT -- potom pozriet dokumentaciu
        temp = GetParamNode(temp);
    }
    return newSymtable;
}

TNode* GetBlockSymtable(ASTNode *node, SymList *list){
    TNode *newSymtable = NULL;
    ASTNode *idNoNullNode = GetNoNullId(node);
    if(idNoNullNode != NULL){
        ASTNode *temp = GetConditionNode(node);
        char *id = GetId(temp);
        //pozriet do tabuliek symbolov -> najst jeho typ a hodit ho do novej symtable
        SymListNode *tempNode = GetLast(list);
        TNode *symtable = GetTableNode(tempNode);
        while(symtable != NULL){
            TNode *match = SearchNode(symtable, id);
            if(match == NULL){
                tempNode = GetNext(tempNode);
                symtable = GetTableNode(tempNode);
                continue;
            }
            //MOZNO BY BOL DOBRY CHECK CI JE NULLABLE KEDZE BY MALA BYT 
            NType type;
            GetType(symtable, id, &type);
            id = GetId(idNoNullNode);
            newSymtable = InsertNode(newSymtable, id);
            SetType(newSymtable, id, DataTypeToNType(type));
            return newSymtable;
        }
        //--------ASI ERROR?
    }
    return newSymtable;
}

int SemanticAnalysis(ASTNode *root){
    ASTStack *stack = createStack();
    ASTNode *currentNode = GetCode(root);
    TNode *symtable = NULL;
    
    //Ulozime vsetky funkcie do tabulky symbolov
    while(currentNode != NULL){
        ASTNode *temp = GetNode(currentNode);
        ASTNodeType type = GetNodeType(temp);
        if(type != TYPE_FUN_DECL){
            printf("error");
            return 10; //ostatní sémantické chyby
        }
        AnalyzeFunDec(temp, &symtable);
        currentNode = GetCode(currentNode);
    }

    int error = CheckForMainFunction(&symtable);
    if(error == -1){
        printf("erorr");
    }

    SymList *symlist = CreateSymList();
    InsertTable(symlist, symtable);

    currentNode = GetCode(root);
    int level = 0;
    while(currentNode != NULL || level != 0){
        if(currentNode == NULL){
            level--;
            currentNode = pop(stack);
            if(currentNode != NULL){
                ASTNodeType type = GetNodeType(currentNode);
                if(type == TYPE_IF){
                    currentNode = pop(stack);
                    continue;
                }
            }
            currentNode = GetCode(currentNode);
            continue;
            //odstranit symtable
        }

        ASTNodeType type = GetNodeType(currentNode);
        switch(type){
            case TYPE_CODE:
                push(stack, currentNode);
                currentNode = GetNode(currentNode);
                level++;
                continue;
            case TYPE_FUN_DECL:
                if(level == 1){
                    //najdi argumenty a pridaj do lokalnej symtable, pre teraz skip
                    symtable = GetFunctionSymtable(currentNode);
                    currentNode = GetNode(currentNode);
                }else{
                    //error - definicia funkcie v definicii funkcie
                }
                break;
            case TYPE_RETURN:
                //check return type funkcie a expression type
                level--;
                currentNode = pop(stack);
                currentNode = GetCode(currentNode);
                break;
            case TYPE_IF_ELSE:
                //IF A ELSE NODY BY MALI BYT VYTVORENE (dufam)
                level++;
                InsertTable(symlist, symtable);
                symtable = GetBlockSymtable(currentNode, symlist);
                push(stack, GetElseNode(currentNode));
                currentNode = GetIfNode(currentNode);
                push(stack,currentNode);
                //check condition
                //check id bez null -> ak ano tak ho pridat do symtable pre IF vetvu
                //checknut if vetvu
                //checknut else vetvu - ako budem vediet ze if je checknute?
                break;
            case TYPE_WHILE:
                //check condition
                //check id bez null -> ak ano tak ho pridat do symtable pre code
                currentNode = GetNode(currentNode);
                break;
            case TYPE_VAR_DECL:
                //check symtable ci existuje
                //check type vs exp type + pridat do symtable
                break;
            case TYPE_ASSIGNMENT:
                //check type vs exp type + constant?
                break;
            case TYPE_FUN_CALL:
                //check symtable ci existuje funkcia
                //check argumenty
                break;
            case TYPE_IF:
                currentNode = GetCode(currentNode);
                break;
            case TYPE_ELSE:
                //vytvorit novu symtable
                currentNode = GetCode(currentNode);
                break;
            default:
                break;
        }
    }
    /*int level,error = 0;
    while(temp != NULL){
        ASTNodeType type = GetNodeType(temp);
        switch(type){
            case TYPE_FUN_DECL:
                if(level != 0){
                    //return error code
                }
                error = AnalyzeFunDec(temp->left);
                break;
            case TYPE_CODE:
                push(stack, temp);
                temp = GetNode(temp);
                break;
            default:
                break;
        }
        //temp = pop(stack);
    }*/

    return 0;
}

ASTNode* createAST(){
    ASTNode *root = CreateAST();

    ASTNode *temp = CreateCodeNode(root);
    temp = CreateFunDeclNode(temp);
    CreateIdNode(temp, "max");
    CreateParamNode(temp, "a");
    CreateTypeNode(temp, T_I32);
    CreateParamNode(temp, "b");
    CreateTypeNode(temp, T_F64);
    CreateParamNode(temp, "c");
    CreateTypeNode(temp, T_F64);
    CreateTypeNode(temp, T_I32);

    /*temp = CreateCodeNode(temp);
    ASTNode *whilee = CreateWhileNode(temp);
    CreateCodeNode(whilee);*/

    temp = CreateCodeNode(temp);
    ASTNode *temp3 = CreateIfElseNode(temp);
    temp3->left->left = (ASTNode *)malloc(sizeof(ASTNode));
    temp3->left->left->type = TYPE_ID;
    temp3->left->left->data.str = strdup("aaaa");
    temp3->left->right = (ASTNode *)malloc(sizeof(ASTNode));
    temp3->left->right->type = TYPE_ID;
    temp3->left->right->data.str = strdup("b");
    ASTNode *temp4 = CreateIfNode(temp3);
    ASTNode *temp5 = CreateElseNode(temp4);

    temp = CreateCodeNode(temp);
    ASTNode *ret = CreateReturnNode(temp);
    ret = CreateIdNode(ret, "a");


    ASTNode *temp2 = CreateCodeNode(root);
    temp2 = CreateFunDeclNode(temp2);
    CreateIdNode(temp2, "main");
    CreateTypeNode(temp2, T_VOID);
    return root;
}


int main(){
    ASTNode *root = createAST();
    SemanticAnalysis(root);
    //DisplayAST(root);
    return 0;
}