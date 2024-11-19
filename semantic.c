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

//NULLABLE TYPY DO SYMTABLE AKO PARAMTYPES?

NType DataTypeToNType(DataType type){
    switch(type){
        case T_F64:
            return TYPE_F64;
        case T_F64_N:
            return TYPE_F64_N;
        case T_VOID:
            return TYPE_VOID;
        case T_I32:
            return TYPE_I32;
        case T_I32_N:
            return TYPE_I32_N;
        case T_U8:
            return TYPE_U8;
        case T_U8_N:
            return TYPE_U8_N;
        default:
            return TYPE_DEFAULT;
    }
}

DataType NTypeToDataType(NType type){
    switch(type){
        case TYPE_F64:
            return T_F64;
        case TYPE_F64_N:
            return T_F64_N;
        case TYPE_VOID:
            return T_VOID;
        case TYPE_I32:
            return T_I32;
        case TYPE_I32_N:
            return T_I32_N;
        case TYPE_U8:
            return T_U8;
        case TYPE_U8_N:
            return T_U8_N;
        default:
            return T_DEFAULT;
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
        printf("errssssor");
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

//IF_ELSE, WHILE
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

TNode* FindInSymlist(SymList *list, char *id){
    TNode *node = NULL;
    SymListNode *currentSymListNode = GetLast(list);
    TNode *symtable = GetTableNode(currentSymListNode);

    while(symtable != NULL){
        node = SearchNode(symtable, id);
        if(node == NULL){
            currentSymListNode = GetNext(currentSymListNode);
            symtable = GetTableNode(currentSymListNode);
        }else{
            break;
        }
    }
    return node;
}

int AnalyzeFunctionCall(ASTNode *node, SymList *list, NType *type){
    ASTNode *idNode = GetIdNode(node);
    char *id = GetId(idNode);
    TNode *tableNode = FindInSymlist(list, id);
    if(tableNode == NULL){
        //ERROR funkcia neexistuje
        return 3;
    }
    
    NType returnType;
    GetFunctionReturnType(tableNode, id, &returnType);
    int paramCount;
    GetParameterCount(tableNode, id, &paramCount);
    NType pt[paramCount];
    GetParameters(tableNode, id, pt);

    ASTNode *temp = GetArgNode(node);
    int argCount = 0;
    while(temp != NULL){
        if(argCount < paramCount){
            ASTNodeType nodeType = GetNodeType(temp);

            if(nodeType == TYPE_ARGUMENT){
                char *argId = GetId(temp);
                TNode *argumentNode = FindInSymlist(list, argId);
                if(argumentNode == NULL){
                    //error nenasiel sa param
                    printf("nenasiel sa param");
                    return 3;
                }
                SetIsUsed(argumentNode, argId);
                NType argtype;
                GetType(argumentNode, argId, &argtype);
                if(argtype == pt[argCount]){
                    argCount++;
                    temp = GetArgNode(temp);
                    continue;
                }
            }else if(nodeType == TYPE_VALUE_I32){
                if(pt[argCount] == TYPE_I32 || pt[argCount] == TYPE_I32_N){
                    argCount++;
                    temp = GetArgNode(temp);
                    continue;
                }
            }else if(nodeType == TYPE_VALUE_F64){
                if(pt[argCount] == TYPE_F64 || pt[argCount] == TYPE_F64_N){
                    argCount++;
                    temp = GetArgNode(temp);
                    continue;
                }
            }else if(nodeType == TYPE_STRING){
                if(pt[argCount] == TYPE_U8 || pt[argCount] == TYPE_U8_N){
                    argCount++;
                    temp = GetArgNode(temp);
                    continue;
                }
            }else{
                if(pt[argCount] == TYPE_I32_N || pt[argCount] == TYPE_F64_N || pt[argCount] == TYPE_U8_N){
                    argCount++;
                    temp = GetArgNode(temp);
                    continue;
                }
            }

            printf("zly param type");
            return 4;
        }else{
            printf("vela");
            return 4;
            //error vela parametrov
        }
    }

    if(paramCount != argCount){
        printf("malo");
        return 4;
        //error malo parametrov
    }

    *type = returnType;
    return 0;
}

int AnalyzeExpression(ASTNode *node, SymList *list, DataType *expType, bool *isKnown){
    if(node == NULL){
        return -1;
    }

    int error = 0;
    ASTNodeType type = GetNodeType(node);
    if(type == TYPE_OPERATOR){
        DataType expTypeL;
        DataType expTypeR;
        bool isKnownL;
        bool isKnownR;
        error = AnalyzeExpression(node->left, list, &expTypeL, &isKnownL);
        if(error){
            return error;
        }
        error = AnalyzeExpression(node->right, list, &expTypeR, &isKnownR);
        if(error){
            return error;
        }
        if(isKnownL && isKnownR){
            *isKnown = true;
        }else{
            *isKnown = false;
        }

        if(expTypeL == T_I32 || expTypeL == T_I32_N){
            if(expTypeR == T_I32 || expTypeR == T_I32_N){
                *expType = T_I32;
                return 0;
            }else if(expTypeR == T_F64 || expTypeR == T_F64_N){
                *expType = T_F64;
                return 0;
            }else{
                return 7;
            }
        }else if(expTypeL == T_F64 || expTypeL == T_F64_N){
            if(expTypeR == T_I32 || expTypeR == T_I32_N){
                *expType = T_F64;
                return 0;
            }else if(expTypeR == T_F64 || expTypeR == T_F64_N){
                *expType = T_F64;
                return 0;
            }else{
                return 7;
            }
        }else{
            return 7;
        }

    }else if(type == TYPE_ID){
        char *id = GetId(node);
        TNode *idNode = FindInSymlist(list, id);
        if(idNode == NULL){
            return 3;
        }

        bool known;
        GetIsKnown(idNode, id, &known);
        if(known){
            *isKnown = true;
        }else{
            *isKnown = false;
        }

        SetIsUsed(idNode, id);
        NType idType;
        GetType(idNode, id, &idType);
        if(idType == TYPE_I32 || idType == TYPE_I32_N){
            *expType = T_I32;
            return 0;
        }else if(idType == TYPE_F64 || idType == TYPE_F64_N){
            *expType = T_F64;
            return 0;
        }else if(idType == TYPE_U8 || idType == TYPE_U8_N){
            *expType = T_U8;
            return 0;
        }else{
            return -1;
        }
    }else if(type == TYPE_VALUE_I32){
        *isKnown = true;
        *expType = T_I32;
        return 0;
    }else if(type == TYPE_VALUE_F64){
        *isKnown = true;
        *expType = T_F64;
        return 0;
    }else if(type == TYPE_STRING){
        return 8;
    }else if(type == TYPE_FUN_CALL){
        *isKnown = false;
        NType funRetType;
        error = AnalyzeFunctionCall(node, list, &funRetType);
        if(error){
            return error;
        }
        *expType = NTypeToDataType(funRetType);
        return 0;
    }else{
        return -1;
    }
    return error;
}

int AnalyzeVariableDeclaration(ASTNode *node, SymList *list){
    ASTNode *idNode = GetIdNode(node);
    char *id = GetId(idNode);
    DataType type = GetDataType(idNode);
    TNode *tableNode = FindInSymlist(list, id);
    int error = 0;

    if(type == T_VOID){
        return -1;
    }

    if(tableNode != NULL){
        return 5;
    }

    ASTNode *temp = GetNode(node);
    if(temp == NULL){
        return -1;
    }

    SymListNode *symtableListNode = GetLast(list);
    TNode *symtable = GetTableNode(symtableListNode);

    ASTNodeType astType = GetNodeType(temp);
    bool isKnown = false;

    if(astType == TYPE_NULL){
        if(type == T_DEFAULT){
            return 8;
        }else if(type == T_F64 || type == T_I32 || type == T_U8){
            return 7;
        }else{
            symtable = InsertNode(symtable, id);
            SetType(symtable, id, DataTypeToNType(type));
        }
    }else{
        DataType expType;
        error = AnalyzeExpression(temp, list, &expType, &isKnown);
        if(error){
            return error;
        }
        if(type == T_DEFAULT){
            symtable = InsertNode(symtable, id);
            SetType(symtable, id, DataTypeToNType(expType));
        }else if(expType == type || (expType == T_I32 && type == T_I32_N) || (expType == T_F64 && type == T_F64_N) || (expType == T_U8 && type == T_U8_N)){
            symtable = InsertNode(symtable, id);
            SetType(symtable, id, DataTypeToNType(type));
        }else{
            return 7;
        }

    }

    if(node->type == TYPE_CON_DECL){
        SetIsConstant(symtable, id, true);
        if(isKnown){
            SetIsKnown(symtable, id);
        }
    }
    symtableListNode->node = symtable; 
    return 0;
}

int AnalyzeAssignment(ASTNode *node, SymList *list){
    ASTNode *idNode = GetIdNode(node);
    char *id = GetId(idNode);
    TNode *tableNode = FindInSymlist(list, id);
    int error = 0;
    bool isConstant = false;
    GetIsConstant(tableNode, id, &isConstant);

    if(isConstant){
        //je konstanta
        return 5;
    }

    if(tableNode == NULL){
        //ERROR premenna neexistuje
        return 3;
    }

    ASTNode *temp = GetNode(node);
    if(temp == NULL){
        //error chyba expression 
        return -1;
    }

    ASTNodeType astType = GetNodeType(temp);
    NType type;
    GetType(tableNode, id, &type);

    if(astType == TYPE_NULL){
        if(type == TYPE_I32_N || type == TYPE_F64_N || type == TYPE_U8_N){
            return 0;
        }else{
            return 7;
        }
    }else{
        DataType expType;
        bool isKnown;
        error = AnalyzeExpression(temp, list, &expType, &isKnown);
        if(error){
            if(error == 8){
                error = 7;
            }
            return error;
        }
        if(expType == type || (expType == T_I32 && type == T_I32_N) || (expType == T_F64 && type == T_F64_N) || (expType == T_U8 && type == T_U8_N)){
            return 0;
        }else{
            return 7;
        }
    }

}


//POZRIET FORUM https://moodle.vut.cz/mod/forum/discuss.php?d=4850
int AnalyzeCondition(ASTNode *node, SymList *list){
    if(node == NULL){
        //error
    }
    ASTNodeType type = GetNodeType(node);
    if(type == TYPE_REL_OPERATOR){
        Operator op = GetOperator(node);
        ASTNode *rightOperand = GetRightOperand(node);
        ASTNode *leftOperand = GetLeftOperand(node);
        ASTNodeType rightNodeType = GetNodeType(rightOperand);
        ASTNodeType leftNodeType = GetNodeType(leftOperand);
        if(op == OP_EQ || op == OP_NEQ){
            if(rightNodeType == TYPE_NULL){
                if(leftNodeType == TYPE_NULL){
                    return 0;
                }else if(leftNodeType == TYPE_ID){
                    char *id = GetId(leftOperand);
                    TNode *symNode = FindInSymlist(list, id);
                    if(symNode == NULL){
                        return 3;
                    }else{
                        NType idType;
                        GetType(symNode, id, &idType);
                        SetIsUsed(symNode, id);
                        if(idType == TYPE_I32_N || idType == TYPE_F64_N || idType == TYPE_U8_N){
                            return 0;
                        }else{
                            return 7;
                        }
                    }
                }else{
                    return 7;
                }
            }else if(rightNodeType == TYPE_VALUE_I32 || rightNodeType == TYPE_VALUE_F64){
                if(leftNodeType == TYPE_ID){
                    char *id = GetId(leftOperand);
                    TNode *symNode = FindInSymlist(list, id);
                    if(symNode == NULL){
                        return 3;
                    }else{
                        NType idType;
                        GetType(symNode, id, &idType);
                        SetIsUsed(symNode, id);
                        if(idType == TYPE_I32 || idType == TYPE_F64 || idType == TYPE_I32_N || idType == TYPE_F64_N){
                            return 0;
                        }else{
                            return 7;
                        }
                    }
                }else if(leftNodeType == TYPE_VALUE_I32 || leftNodeType == TYPE_VALUE_F64){
                    return 0;
                }else{
                    return 7;
                }
            }else if(rightNodeType == TYPE_ID){
                char *id = GetId(rightOperand);
                TNode *symNode = FindInSymlist(list, id);
                if(symNode == NULL){
                    return 3;
                }else{
                    NType idType;
                    GetType(symNode, id, &idType);
                    SetIsUsed(symNode, id);
                    bool isConst = false;
                    GetIsConstant(symNode, id, &isConst);
                    if(idType == TYPE_U8){
                        return 7;
                    }else if(idType == TYPE_U8_N){
                        if(leftNodeType == TYPE_NULL){
                            return 0;
                        }else{
                            return 7;
                        }
                    }else{
                        if(leftNodeType == TYPE_ID){
                            char *idLeft = GetId(leftOperand);
                            TNode *leftSymNode = FindInSymlist(list, idLeft);
                            if(leftSymNode == NULL){
                                return 3;
                            }
                            NType idLeftType;
                            GetType(leftSymNode, idLeft, &idLeftType);
                            SetIsUsed(leftSymNode, idLeft);
                            bool isConstLeft = false;
                            GetIsConstant(leftSymNode, idLeft, &isConstLeft);
                            if((idLeftType == TYPE_I32 || idLeftType == TYPE_I32_N) && (idType == TYPE_I32 || idType == TYPE_I32_N)){
                                return 0;
                            }else if((idLeftType == TYPE_F64 || idLeftType == TYPE_F64_N) && (idType == TYPE_F64 || idType == TYPE_F64_N)){
                                return 0;
                            }else if(isConst && isConstLeft){
                                return 0;
                            }else{
                                return 7;
                            }
                        }else if(leftNodeType == TYPE_NULL){
                            if(idType == TYPE_I32_N || idType == TYPE_F64_N){
                                return 0;
                            }else{
                                return 7;
                            }
                        }else if(leftNodeType == TYPE_VALUE_I32 || leftNodeType == TYPE_VALUE_F64){
                            if(idType == TYPE_I32 || idType == TYPE_I32_N){
                                if(leftNodeType == TYPE_VALUE_I32){
                                    return 0;
                                }else{
                                    if(isConst){
                                        return 0;
                                    }
                                    else{
                                        return 7;
                                    }
                                }
                            }else{
                                if(leftNodeType == TYPE_VALUE_F64){
                                    return 0;
                                }else{
                                    if(isConst){
                                        return 0;
                                    }
                                    else{
                                        return 7;
                                    }
                                }
                            }
                        }else{
                            return 7;  
                        }
                    }
                }
            }else{
                return 7;
            }
        }else{
            if(leftNodeType == TYPE_NULL || rightNodeType == TYPE_NULL){
                return 7;
            }
            
            if(rightNodeType == TYPE_ID){
                char *id = GetId(rightOperand);
                TNode *symNode = FindInSymlist(list, id);
                if(symNode == NULL){
                    return 3;
                }
                NType idType;
                GetType(symNode, id, &idType);
                SetIsUsed(symNode, id);

                if(idType == TYPE_I32_N || idType == TYPE_F64_N || idType == TYPE_U8_N || idType == TYPE_U8){
                    return 7;
                }

                if(leftNodeType == TYPE_ID){
                    char *idLeft = GetId(leftOperand);
                    TNode *symLeftNode = FindInSymlist(list, idLeft);
                    if(symLeftNode == NULL){
                        return 3;
                    }
                    NType idLeftType;
                    GetType(symLeftNode, idLeft, &idLeftType);
                    SetIsUsed(symLeftNode, idLeft);
                    if(idType == idLeftType){
                        return 0;
                    }else{
                        return 7;
                    }
                }else if(leftNodeType == TYPE_VALUE_I32){
                    if(idType == TYPE_I32 || idType == TYPE_F64){
                        return 0;
                    }else{
                        return 7;
                    }
                }else if(leftNodeType == TYPE_VALUE_F64){
                    if(idType == TYPE_F64){
                        return 0;
                    }else{
                        return 7;
                    }
                }else{
                    return 7;
                }
            }else if(rightNodeType == TYPE_VALUE_I32 || rightNodeType == TYPE_VALUE_F64){
                if(leftNodeType == TYPE_ID){

                }else if(leftNodeType == TYPE_VALUE_I32 || leftNodeType == TYPE_VALUE_F64){
                    //ZALEZI CI JE TO POVOLENE ALEBO NIE
                    return 0;
                }else{
                    return 7;
                }
            }else{
                return 7;
            }
        }
    }else if(type == TYPE_NULL){

    }
    return 0;
}

int SemanticAnalysis(ASTNode *root){
    ASTStack *stack = CreateStackAST();
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
            currentNode = PopAST(stack);
            if(currentNode != NULL){
                ASTNodeType type = GetNodeType(currentNode);
                if(type == TYPE_IF_CLOSED){
                    currentNode = PopAST(stack);
                    PushAST(stack, currentNode);
                    DeleteTable(symlist); 
                    continue;
                }
                if(type == TYPE_ELSE_CLOSED){
                    DeleteTable(symlist); 
                    SymListNode *symListNode = GetLast(symlist);
                    symtable = GetTableNode(symListNode);
                    currentNode = PopAST(stack);
                }
                if(type == TYPE_FUN_DECL){
                    DeleteTable(symlist);
                    SymListNode *symListNode = GetLast(symlist);
                    symtable = GetTableNode(symListNode);
                    currentNode = PopAST(stack);
                }
                if(type == TYPE_WHILE_CLOSED){
                    DeleteTable(symlist);
                    SymListNode *symListNode = GetLast(symlist);
                    symtable = GetTableNode(symListNode);
                    currentNode = PopAST(stack);
                }
            }
            currentNode = GetCode(currentNode);
            continue;
            //odstranit symtable
        }

        ASTNodeType type = GetNodeType(currentNode);
        switch(type){
            case TYPE_CODE:
                PushAST(stack, currentNode);
                currentNode = GetNode(currentNode);
                level++;
                continue;
            case TYPE_FUN_DECL:
                if(level == 1){
                    //najdi argumenty a pridaj do lokalnej symtable, pre teraz skip
                    symtable = GetFunctionSymtable(currentNode);
                    InsertTable(symlist, symtable);
                    PushAST(stack, currentNode);
                    currentNode = GetNode(currentNode);
                }else{
                    //error - definicia funkcie v definicii funkcie
                    return 10;
                }
                break;
            case TYPE_RETURN:
                printf("IN RETURN NODE\n");
                //check return type funkcie a expression type
                level--;
                currentNode = PopAST(stack);
                currentNode = GetCode(currentNode);
                break;
            case TYPE_IF_ELSE:
                level++;
                symtable = GetBlockSymtable(currentNode, symlist);
                InsertTable(symlist, symtable);
                PushAST(stack, GetElseNode(currentNode));
                currentNode = GetIfNode(currentNode);
                PushAST(stack,currentNode);
                //check condition
                break;
            case TYPE_WHILE_CLOSED:
                //check condition
                symtable = GetBlockSymtable(currentNode, symlist);
                InsertTable(symlist, symtable);
                PushAST(stack, currentNode);
                currentNode = GetNode(currentNode);
                break;
            case TYPE_VAR_DECL:
            case TYPE_CON_DECL:
                error = AnalyzeVariableDeclaration(currentNode, symlist);
                if(error){
                    //vycistit pamat este
                    return error;
                }
                SymListNode *symListNode = GetLast(symlist);
                symtable = GetTableNode(symListNode);
                currentNode = PopAST(stack);
                currentNode = GetCode(currentNode);
                level--;
                break;
            case TYPE_ASSIGNMENT:
                error = AnalyzeAssignment(currentNode, symlist);
                if(error){
                    //vycistit pamat este
                    return error;
                }
                currentNode = PopAST(stack);
                currentNode = GetCode(currentNode);
                level--;
                break;
            case TYPE_FUN_CALL:
                NType funReturnType;
                error = AnalyzeFunctionCall(currentNode, symlist, &funReturnType);
                if(error){
                    //vycistit pamat este
                    return error;
                }
                currentNode = PopAST(stack);
                currentNode = GetCode(currentNode);
                level--;
                //check symtable ci existuje funkcia
                //check argumenty
                break;
            case TYPE_IF_CLOSED:
                currentNode = GetCode(currentNode);
                break;
            case TYPE_ELSE_CLOSED:
                symtable = NULL;
                InsertTable(symlist, symtable);
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

    ASTNode *temp2 = CreateCodeNode(root);
    temp2 = CreateFunDeclNode(temp2);
    CreateIdNode(temp2, "main");
    CreateTypeNode(temp2, T_VOID);
    temp2 = CreateCodeNode(temp2);
    ASTNode *var2 = CreateVarDeclNode(temp2);
    CreateIdNode(var2, "a");
    CreateTypeNode(var2, T_I32);
    var2->right = CreateAstNode(TYPE_VALUE_I32);
    /*ASTNode *temp = CreateCodeNode(root);
    temp = CreateFunDeclNode(temp);
    CreateIdNode(temp, "max");
    CreateParamNode(temp, "a");
    CreateTypeNode(temp, T_F64);
    CreateParamNode(temp, "b");
    CreateTypeNode(temp, T_F64);
    CreateParamNode(temp, "c");
    CreateTypeNode(temp, T_F64);
    CreateTypeNode(temp, T_I32);*/

    /*temp = CreateCodeNode(temp);
    ASTNode *whilee = CreateWhileNode(temp);
    whilee->type = TYPE_WHILE_CLOSED;*/

    //var decl
    /*temp = CreateCodeNode(temp);
    ASTNode *var = CreateConDeclNode(temp);
    CreateIdNode(var, "aa");*/
    
    /*CreateTypeNode(var, T_I32_N);
    var->right = CreateAstNode(TYPE_OPERATOR);
    var->right->right = CreateAstNode(TYPE_VALUE_I32);
    var->right->left = CreateAstNode(TYPE_VALUE_I32);*/

    /*temp = CreateCodeNode(temp);
    ASTNode *var1 = CreateVarDeclNode(temp);
    CreateIdNode(var1, "ab");*/
    
    /*CreateTypeNode(var1, T_I32_N);
    var1->right = CreateAstNode(TYPE_OPERATOR);
    var1->right->right = CreateAstNode(TYPE_VALUE_I32);
    var1->right->left = CreateAstNode(TYPE_VALUE_I32);*/

    /*temp = CreateCodeNode(temp);
    ASTNode *var2 = CreateVarDeclNode(temp);
    CreateIdNode(var2, "aa");*/
    
    /*CreateTypeNode(var2, T_I32_N);
    var2->right = CreateAstNode(TYPE_OPERATOR);
    var2->right->right = CreateAstNode(TYPE_VALUE_I32);
    var2->right->left = CreateAstNode(TYPE_VALUE_I32);*/
    /*ASTNode *fc = CreateFunCallNode(var);
    CreateIdNode(fc, "max");
    CreateArgumentNode(fc,"a");
    CreateArgumentNode(fc,"a");
    CreateArgumentNode(fc,"a");*/

    /*temp = CreateCodeNode(temp);
    ASTNode *fc1 = CreateFunCallNode(temp);
    CreateIdNode(fc1, "max");
    CreateArgumentNode(fc1,"a");
    CreateArgumentNode(fc1,"a");
    CreateArgumentNode(fc1,"a");*/


    /*temp = CreateCodeNode(temp);
    ASTNode *temp3 = CreateIfElseNode(temp);
    temp3->left->left = (ASTNode *)malloc(sizeof(ASTNode));
    temp3->left->left->type = TYPE_ID;
    temp3->left->left->data.str = strdup("aaaa");
    temp3->left->right = (ASTNode *)malloc(sizeof(ASTNode));
    temp3->left->right->type = TYPE_ID;
    temp3->left->right->data.str = strdup("b");
    ASTNode *temp4 = CreateIfNode(temp3);
    ASTNode *temp5 = CreateElseNode(temp3);
    temp4->left->type = TYPE_IF_CLOSED;
    temp5->right->right->type = TYPE_ELSE_CLOSED;*/

    /*temp = CreateCodeNode(temp);
    ASTNode *ret = CreateReturnNode(temp);
    ret = CreateIdNode(ret, "a");*/

    return root;
}


int main(){
    ASTNode *root = createAST();
    int a = SemanticAnalysis(root);
    printf("%d", a);
    return 0;
}