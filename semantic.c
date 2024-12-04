/**
 * @file semantic.c
 * @author Robin Kurilla
 * @brief  source file for semantic analysis
 */

#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "error.h"

/**
 * @brief converts DataType to NType
 * @param type type to be converted
 * @return corresponding NType
 */
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

/**
 * @brief converts NType to DataType
 * @param type type to be converted
 * @return corresponding DataType
 */
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

/**
 * @brief frees resources
 * @param stack ASTStack to be freed
 * @param list SymList to be freed
 */
void FreeSemantics(ASTStack *stack, SymList *list){
    FreeStackAST(stack);
    FreeSymlist(list);
}

/**
 * @brief adds built in functions into symtable
 * @param symtable symtable in which we add built in functions
 */
void AddBuiltInFunctionsToSymtable(TNode **symtable){
    char *id = "ifj.readstr";
    *symtable = InsertNode(*symtable, id);
    SetType(*symtable, id, TYPE_FUNCTION);
    SetFunctionReturnType(*symtable, id, TYPE_U8_N);

    id = "ifj.readf64";
    *symtable = InsertNode(*symtable, id);
    SetType(*symtable, id, TYPE_FUNCTION);
    SetFunctionReturnType(*symtable, id, TYPE_F64_N);

    id = "ifj.readi32";
    *symtable = InsertNode(*symtable, id);
    SetType(*symtable, id, TYPE_FUNCTION);
    SetFunctionReturnType(*symtable, id, TYPE_I32_N);

    id = "ifj.write";
    *symtable = InsertNode(*symtable, id);
    SetType(*symtable, id, TYPE_FUNCTION);
    SetFunctionReturnType(*symtable, id, TYPE_VOID);

    id = "ifj.i2f";
    *symtable = InsertNode(*symtable, id);
    SetType(*symtable, id, TYPE_FUNCTION);
    SetFunctionReturnType(*symtable, id, TYPE_F64);
    SetParameter(*symtable, id, TYPE_I32);

    id = "ifj.f2i";
    *symtable = InsertNode(*symtable, id);
    SetType(*symtable, id, TYPE_FUNCTION);
    SetFunctionReturnType(*symtable, id, TYPE_I32);
    SetParameter(*symtable, id, TYPE_F64);

    id = "ifj.string";
    *symtable = InsertNode(*symtable, id);
    SetType(*symtable, id, TYPE_FUNCTION);
    SetFunctionReturnType(*symtable, id, TYPE_U8);
    
    id = "ifj.length";
    *symtable = InsertNode(*symtable, id);
    SetType(*symtable, id, TYPE_FUNCTION);
    SetFunctionReturnType(*symtable, id, TYPE_I32);
    SetParameter(*symtable, id, TYPE_U8);

    id = "ifj.concat";
    *symtable = InsertNode(*symtable, id);
    SetType(*symtable, id, TYPE_FUNCTION);
    SetFunctionReturnType(*symtable, id, TYPE_U8);
    SetParameter(*symtable, id, TYPE_U8);
    SetParameter(*symtable, id, TYPE_U8);

    id = "ifj.substring";
    *symtable = InsertNode(*symtable, id);
    SetType(*symtable, id, TYPE_FUNCTION);
    SetFunctionReturnType(*symtable, id, TYPE_U8_N);
    SetParameter(*symtable, id, TYPE_U8);
    SetParameter(*symtable, id, TYPE_I32);
    SetParameter(*symtable, id, TYPE_I32);

    id = "ifj.strcmp";
    *symtable = InsertNode(*symtable, id);
    SetType(*symtable, id, TYPE_FUNCTION);
    SetFunctionReturnType(*symtable, id, TYPE_I32);
    SetParameter(*symtable, id, TYPE_U8);
    SetParameter(*symtable, id, TYPE_U8);

    id = "ifj.ord";
    *symtable = InsertNode(*symtable, id);
    SetType(*symtable, id, TYPE_FUNCTION);
    SetFunctionReturnType(*symtable, id, TYPE_I32);
    SetParameter(*symtable, id, TYPE_U8);
    SetParameter(*symtable, id, TYPE_I32);

    id = "ifj.chr";
    *symtable = InsertNode(*symtable, id);
    SetType(*symtable, id, TYPE_FUNCTION);
    SetFunctionReturnType(*symtable, id, TYPE_U8);
    SetParameter(*symtable, id, TYPE_I32);
}

/**
 * @brief recursively checks for return nodes in function 
 * @param node node that is checked
 * @return 1 if node is TYPE_RETURN or both if else branches has return node, else 0 or calls itself with next node
 */
int LookForReturnNodes(ASTNode *node){
    if(node == NULL){
        return 0;
    }

    ASTNode *temp = GetNode(node);
    if(temp == NULL){
        return 0;
    }
    ASTNodeType type = GetNodeType(temp);
    if(type == TYPE_RETURN){
        return 1;
    }else if(type == TYPE_WHILE_CLOSED){
        int whileNode = LookForReturnNodes(GetNode(temp));
        if(whileNode){
            return whileNode;
        }
    }else if(type == TYPE_IF_ELSE){
        int ifNode = LookForReturnNodes(GetCode(GetIfNode(temp)));
        int elseNode = LookForReturnNodes(GetCode(GetElseNode(temp)));
        if(ifNode && elseNode){
            return 1;
        }
    }

    temp = GetCode(node);
    if(temp == NULL){
        return 0;
    }else{
        return LookForReturnNodes(temp);
    }
}

/**
 * @brief analyzes function declaration and adds function into symtable
 * @param node node that is checked
 * @param symtable symbol table
 * @return 0 if successful, error code if not
 */
int AnalyzeFunDec(ASTNode *node, TNode **symtable){
    if(node == NULL || node->type != TYPE_FUN_DECL){
        return ERROR_SEMANTIC_OTHERS;
    }
    ASTNode *temp = GetIdNode(node);
    char *idFunction = GetId(temp);

    TNode *exists = SearchNode(*symtable, idFunction);
    if(exists){
        return ERROR_REDEFINITION;
    }
    *symtable = InsertNode(*symtable,idFunction);
    SetType(*symtable, idFunction, TYPE_FUNCTION);
    DataType type = GetDataType(temp);

    if(type != T_VOID){
        int ret = LookForReturnNodes(GetNode(node));
        if(!ret){
            return ERROR_BAD_RETURN;
        }
    }

    SetFunctionReturnType(*symtable, idFunction, DataTypeToNType(type));
    temp = GetParamNode(temp);
    while(temp != NULL){
        type = GetDataType(temp);
        SetParameter(*symtable, idFunction, DataTypeToNType(type));
        temp = GetParamNode(temp);
    }

    return 0;

}

/**
 * @brief checks from symtable if main function is in right format
 * @param symtable symbol table
 * @return 0 if successful, error code if not
 */
int CheckForMainFunction(TNode **symtable){
    NType type;
    int error = GetType(*symtable, "main", &type);
    if(error == -1 || type != TYPE_FUNCTION){
        return ERROR_UNDEFINED;
    }
    int count;
    error = GetParameterCount(*symtable, "main", &count);
    if(error == -1 || count != 0){
        return ERROR_PARAMTERS;
    }
    error = GetFunctionReturnType(*symtable, "main", &type);
    if(error == -1 || type != TYPE_VOID){
        return ERROR_PARAMTERS;
    }
    return 0;
}

/**
 * @brief creates symtable when enteting function definition
 * @param node node from which we further get params
 * @param retType function return type 
 * @return new symtable
 */
TNode* GetFunctionSymtable(ASTNode *node, DataType *retType){
    TNode *newSymtable = NULL;
    ASTNode *idNode = GetIdNode(node);
    ASTNode *temp = GetParamNode(idNode);
    while(temp != NULL){
        char *id = GetId(temp);
        newSymtable = InsertNode(newSymtable, id);
        DataType type = GetDataType(temp);
        SetType(newSymtable, id, DataTypeToNType(type));
        SetIsConstant(newSymtable, id, true);
        temp = GetParamNode(temp);
    }

    DataType type = GetDataType(idNode);
    *retType = type;
    return newSymtable;
}

/**
 * @brief looks for node in symtables that are in SymList
 * @param list SymList
 * @param id id of record that we are looking for
 * @return found TNode or NULL
 */
TNode* FindInSymlist(SymList *list, char *id){
    TNode *node = NULL;
    SymListNode *currentSymListNode = GetLast(list);
    TNode *symtable = GetTableNode(currentSymListNode);

    node = SearchNode(symtable, id);
    while(node == NULL){
        currentSymListNode = GetNext(currentSymListNode);
        if(currentSymListNode == NULL)
        {
            break;
        }
        symtable = GetTableNode(currentSymListNode);
        node = SearchNode(symtable, id);
    }
    return node;
}

/**
 * @brief creates symtable when entering blocks (if,else,while)
 * @param node node from which we get id without null if exists
 * @param list SymList
 * @param error error code
 * @return new symtable
 */
TNode* GetBlockSymtable(ASTNode *node, SymList *list, int *error){
    TNode *newSymtable = NULL;
    ASTNode *idNoNullNode = GetNoNullId(node);
    if(idNoNullNode != NULL){
        ASTNode *temp = GetConditionNode(node);
        ASTNodeType type = GetNodeType(temp);
        if(type != TYPE_ID){
            *error = 7;
            return NULL;
        }
        char *id = GetId(temp);

        TNode *symNode = FindInSymlist(list, id);
        if(symNode == NULL){
            *error = 3;
            return NULL;
        }
        
        NType tableType;
        GetType(symNode, id, &tableType);
        if(tableType == TYPE_I32_N || tableType == TYPE_F64_N || tableType == TYPE_U8_N){
            *error = 0;
            NType newType;
            if(tableType == TYPE_I32_N){
                newType = TYPE_I32;
            }else if(tableType == TYPE_F64_N){
                newType = TYPE_F64;
            }else{
                newType = TYPE_U8;
            }
            id = GetId(idNoNullNode);
            newSymtable = InsertNode(newSymtable, id);
            SetType(newSymtable, id, newType);
        }else{
            *error = 7;
        }
        
        return newSymtable;
    }
    return newSymtable;
}

/**
 * @brief recursively analyzes expressions
 * @param node node that is checked
 * @param list SymList
 * @param expType expression type
 * @param isKnown if value is known when compiling
 * @return 0 if successful, error code if not
 */
int AnalyzeExpression(ASTNode *node, SymList *list, DataType *expType, bool *isKnown){
    if(node == NULL){
        return INTERNAL_ERROR;
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

        if(expTypeL == T_I32){
            if(expTypeR == T_I32){
                *expType = T_I32;
                return 0;
            }else if(expTypeR == T_F64){
                *expType = T_F64;
                return 0;
            }else{
                return ERROR_TYPE_COMPABILITY;
            }
        }else if(expTypeL == T_F64){
            if(expTypeR == T_I32){
                *expType = T_F64;
                return 0;
            }else if(expTypeR == T_F64){
                *expType = T_F64;
                return 0;
            }else{
                return ERROR_TYPE_COMPABILITY;
            }
        }else{
            return ERROR_TYPE_COMPABILITY;
        }

    }else if(type == TYPE_ID){
        char *id = GetId(node);
        TNode *idNode = FindInSymlist(list, id);
        if(idNode == NULL){
            return ERROR_UNDEFINED;
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
        if(idType == TYPE_I32){
            *expType = T_I32;
            return 0;
        }else if(idType == TYPE_F64){
            *expType = T_F64;
            return 0;
        }else{
            return ERROR_TYPE_COMPABILITY;
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
        return ERROR_TYPE_NOT_DEFINED;
    }else{
        return ERROR_TYPE_COMPABILITY;
    }
    return error;
}

/**
 * @brief analyzes function call
 * @param node highest node from which we analyze
 * @param list SymList
 * @param type returns function return type
 * @return 0 if successful, error code if not
 */
int AnalyzeFunctionCall(ASTNode *node, SymList *list, NType *type){
    ASTNode *idNode = GetIdNode(node);
    char *id = GetId(idNode);
    TNode *tableNode = FindInSymlist(list, id);
    if(tableNode == NULL){
        return ERROR_UNDEFINED;
    }
    
    NType returnType;
    GetFunctionReturnType(tableNode, id, &returnType);
    int paramCount;
    GetParameterCount(tableNode, id, &paramCount);
    NType pt[paramCount];
    GetParameters(tableNode, id, pt);

    *type = returnType;

    if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.string") || !strcmp(id, "ifj.i2f") || !strcmp(id, "ifj.f2i")){
        ASTNode *special = GetArgNode(node);
        ASTNodeType nodeType = GetNodeType(special);
        if(nodeType == TYPE_NULL){
            if(!strcmp(id, "ifj.write")){
                return 0;
            }else{
                return ERROR_PARAMTERS;
            }
        }else if(nodeType == TYPE_STRING){
            if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.string")){
                return 0;
            }else{
                return ERROR_PARAMTERS;
            }
        }else if(nodeType == TYPE_VALUE_I32){
            if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.i2f")){
                return 0;
            }else{
                return ERROR_PARAMTERS;
            }
        }else if(nodeType == TYPE_VALUE_F64){
            if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.f2i")){
                return 0;
            }else{
                return ERROR_PARAMTERS;
            }
        }else if(nodeType == TYPE_ARGUMENT){
            char *argId = GetId(special);
            TNode *argumentNode = FindInSymlist(list, argId);
            if(argumentNode == NULL){
                return ERROR_UNDEFINED;
            }
            SetIsUsed(argumentNode, argId);
            NType argtype;
            GetType(argumentNode, argId, &argtype);
            if(argtype == TYPE_I32){
                if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.i2f")){
                    return 0;
                }else{
                    return ERROR_PARAMTERS;
                }
            }else if(argtype == TYPE_I32_N){
                if(!strcmp(id, "ifj.write")){
                    return 0;
                }else{
                    return ERROR_PARAMTERS;
                }
            }else if(argtype == TYPE_F64){
                if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.f2i")){
                    return 0;
                }else{
                    return ERROR_PARAMTERS;
                }
            }else if(argtype == TYPE_F64_N){
                if(!strcmp(id, "ifj.write")){
                    return 0;
                }else{
                    return ERROR_PARAMTERS;
                }
            }else if(argtype == TYPE_U8){
                if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.string")){
                    return 0;
                }else{
                    return ERROR_PARAMTERS;
                }
            }else if(argtype == TYPE_U8_N){
                if(!strcmp(id, "ifj.write")){
                    return 0;
                }else{
                    return ERROR_PARAMTERS;
                }
            }
        }else{
            return ERROR_PARAMTERS;
        }
    }


    ASTNode *temp = GetArgNode(node);
    int argCount = 0;
    while(temp != NULL){
        if(argCount < paramCount){
            ASTNodeType nodeType = GetNodeType(temp);

            if(nodeType == TYPE_ARGUMENT){
                char *argId = GetId(temp);
                TNode *argumentNode = FindInSymlist(list, argId);
                if(argumentNode == NULL){
                    return ERROR_UNDEFINED;
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

            return ERROR_PARAMTERS;
        }else{
            return ERROR_PARAMTERS;
        }
    }

    if(paramCount != argCount){
        return ERROR_PARAMTERS;
    }

    return 0;
}

/**
 * @brief analyzes variable/constant declaration
 * @param node highest node from which we analyze further
 * @param list SymList
 * @return 0 if successful, error code if not
 */
int AnalyzeVariableDeclaration(ASTNode *node, SymList *list){
    ASTNode *idNode = GetIdNode(node);
    char *id = GetId(idNode);

    if(!strcmp(id, "_")){
        return ERROR_SEMANTIC_OTHERS;
    }

    DataType type = GetDataType(idNode);
    TNode *tableNode = FindInSymlist(list, id);
    int error = 0;

    if(type == T_VOID){
        return ERROR_SEMANTIC_OTHERS;
    }

    if(tableNode != NULL){
        return ERROR_REDEFINITION;
    }

    ASTNode *temp = GetNode(node);
    if(temp == NULL){
        return INTERNAL_ERROR;
    }

    SymListNode *symtableListNode = GetLast(list);
    TNode *symtable = GetTableNode(symtableListNode);

    ASTNodeType astType = GetNodeType(temp);
    bool isKnown = false;

    if(astType == TYPE_NULL){
        if(type == T_DEFAULT){
            return ERROR_TYPE_NOT_DEFINED;
        }else if(type == T_F64 || type == T_I32 || type == T_U8){
            return ERROR_TYPE_COMPABILITY;
        }else{
            symtable = InsertNode(symtable, id);
            SetType(symtable, id, DataTypeToNType(type));
        }
    }else if(astType == TYPE_OPERATOR || astType == TYPE_VALUE_I32 || astType == TYPE_VALUE_F64){
        DataType expType;
        error = AnalyzeExpression(temp, list, &expType, &isKnown);
        if(error){
            return error;
        }
        if(type == T_DEFAULT){
            symtable = InsertNode(symtable, id);
            SetType(symtable, id, DataTypeToNType(expType));
        }else if(type == expType || (type == T_I32_N && expType == T_I32) || (type == T_F64_N && expType == T_F64)){
            symtable = InsertNode(symtable, id);
            SetType(symtable, id, DataTypeToNType(type));
        }else{
            return ERROR_TYPE_COMPABILITY;
        }

    }else if(astType == TYPE_ID || astType == TYPE_FUN_CALL){
        NType rightType;
        if(astType == TYPE_ID){
            char *idRight = GetId(temp);
            TNode *idRightTableNode = FindInSymlist(list, idRight);
            if(idRightTableNode == NULL){
                return ERROR_UNDEFINED;
            }
            SetIsUsed(idRightTableNode, idRight);
            GetType(idRightTableNode, idRight, &rightType);
            GetIsKnown(idRightTableNode, idRight, &isKnown);
        }else{
            error = AnalyzeFunctionCall(temp, list, &rightType);
            if(error){
                return error;
            }
            if(rightType == TYPE_VOID){
                return ERROR_TYPE_COMPABILITY;
            }
        }
    
        if(type == T_DEFAULT){
            symtable = InsertNode(symtable, id);
            SetType(symtable, id, rightType);
        }else if(type == T_I32){
            if(rightType == TYPE_I32){
                symtable = InsertNode(symtable, id);
                SetType(symtable, id, rightType);
            }else{
                return ERROR_TYPE_COMPABILITY;
            }
        }else if(type == T_I32_N){
            if(rightType == TYPE_I32 || rightType == TYPE_I32_N){
                symtable = InsertNode(symtable, id);
                SetType(symtable, id, TYPE_I32_N);
            }else{
                return ERROR_TYPE_COMPABILITY;
            }
        }else if(type == T_F64){
            if(rightType == TYPE_F64){
                symtable = InsertNode(symtable, id);
                SetType(symtable, id, rightType);
            }else{
                return ERROR_TYPE_COMPABILITY;
            }
        }else if(type == T_F64_N){
            if(rightType == TYPE_F64 || rightType == TYPE_F64_N){
                symtable = InsertNode(symtable, id);
                SetType(symtable, id, TYPE_F64_N);
            }else{
                return ERROR_TYPE_COMPABILITY;
            }
        }else if(type == T_U8){
            if(rightType == TYPE_U8){
                symtable = InsertNode(symtable, id);
                SetType(symtable, id, rightType);
            }else{
                return ERROR_TYPE_COMPABILITY;
            }
        }else if(type == T_U8_N){
            if(rightType == TYPE_U8 || rightType == TYPE_U8_N){
                symtable = InsertNode(symtable, id);
                SetType(symtable, id, TYPE_U8_N);
            }else{
                return ERROR_TYPE_COMPABILITY;
            }
        }
    }else if(astType == TYPE_FUN_CALL){
        NType rightType;
        AnalyzeFunctionCall(temp, list, &rightType);
    }else{
        return ERROR_TYPE_COMPABILITY;
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

/**
 * @brief analyzes assignment
 * @param node highest node from which we analyze
 * @param list SymList
 * @return 0 if successful, error code if not
 */
int AnalyzeAssignment(ASTNode *node, SymList *list){
    ASTNode *idNode = GetIdNode(node);
    char *id = GetId(idNode);
    int error = 0;
    TNode *tableNode;
    bool underscore = false;
    if(!strcmp(id, "_")){
        underscore = true;
    }else{
        tableNode = FindInSymlist(list, id);
        bool isConstant = false;
        GetIsConstant(tableNode, id, &isConstant);

        if(isConstant){
            return ERROR_REDEFINITION;
        }

        if(tableNode == NULL){
            return ERROR_UNDEFINED;
        }
    }

    ASTNode *temp = GetNode(node);
    if(temp == NULL){
        return INTERNAL_ERROR;
    }

    ASTNodeType astType = GetNodeType(temp);
    NType type;
    if(!underscore){
        GetType(tableNode, id, &type);
    }
    bool isKnown;

    if(astType == TYPE_NULL){
        if(underscore){
            return 0;
        }
        if(type == TYPE_I32_N || type == TYPE_F64_N || type == TYPE_U8_N){
            return 0;
        }else{
            return ERROR_TYPE_COMPABILITY;
        }
    }else if(astType == TYPE_OPERATOR || astType == TYPE_VALUE_I32 || astType == TYPE_VALUE_F64){
        DataType expType;
        error = AnalyzeExpression(temp, list, &expType, &isKnown);
        if(error){
            return error;
        }

        if(underscore){
            return 0;
        }

        if(type == TYPE_I32 || type == TYPE_I32_N){
            if(expType == T_I32){
                return 0;
            }else{
                float f = GetFloatValue(temp);
                if(f == floorf(f)){
                    return 0;
                }
                return ERROR_TYPE_COMPABILITY;
            }
        }else if(type == TYPE_F64 || type == TYPE_F64_N){
            if(expType == T_F64){
                return 0;
            }else{
                return ERROR_TYPE_COMPABILITY;
            }
        }
    }else if(astType == TYPE_ID || astType == TYPE_FUN_CALL){
        NType rightType;
        if(astType == TYPE_ID){
            char *idRight = GetId(temp);
            TNode *idRightTableNode = FindInSymlist(list, idRight);
            if(idRightTableNode == NULL){
                return ERROR_UNDEFINED;
            }
            SetIsUsed(idRightTableNode, idRight);
            GetType(idRightTableNode, idRight, &rightType);
            GetIsKnown(idRightTableNode, idRight, &isKnown);
        }else{
            error = AnalyzeFunctionCall(temp, list, &rightType);
            if(error){
                return error;
            }
            if(rightType == TYPE_VOID){
                return ERROR_TYPE_COMPABILITY;
            }
        }

        if(underscore){
            return 0;
        }

        if(type == rightType){
            return 0;
        }else if(type == TYPE_I32_N && rightType == TYPE_I32){
            return 0;
        }else if(type == TYPE_F64_N && rightType == TYPE_F64){
            return 0;
        }else if(type == TYPE_U8_N && rightType == TYPE_U8){
            return 0;
        }else{
            return ERROR_TYPE_COMPABILITY;
        }
    }else{
        return ERROR_TYPE_COMPABILITY;
    }
    
   return 0;
}

/**
 * @brief analyzes condition
 * @param node highest node from which we analyze
 * @param list SymList
 * @param hasNullIde returns true if in condition is variable that is nullable
 * @return 0 if successful, error code if not
 */
int AnalyzeCondition(ASTNode *node, SymList *list, bool *hasNullId){
    int error;
    *hasNullId = false;

    ASTNodeType type = GetNodeType(node);
    if(type == TYPE_REL_OPERATOR){
        Operator op = GetOperator(node);

        ASTNode *leftOperand = GetLeftOperand(node);
        ASTNode *rightOperand = GetRightOperand(node);

        ASTNodeType leftType = GetNodeType(leftOperand);
        ASTNodeType rightType = GetNodeType(rightOperand);

        if(leftType == TYPE_NULL){
            if(rightType == TYPE_NULL){
                if(op == OP_EQ || op == OP_NEQ){
                    return 0;
                }
                return ERROR_TYPE_COMPABILITY;
            }else if(rightType == TYPE_ID){
                if(op == OP_EQ || op == OP_NEQ){
                    DataType idType;
                    bool known;
                    error = AnalyzeExpression(rightOperand, list, &idType, &known);
                    if(error){
                        return error;
                    }
                    if(idType == T_I32_N || idType == T_F64_N || idType == T_U8_N){
                        return 0;
                    }else{
                        return ERROR_TYPE_COMPABILITY;
                    }
                }
                return ERROR_TYPE_COMPABILITY;
            }else{
                return ERROR_TYPE_COMPABILITY;
            }
        }else if(rightType == TYPE_NULL){
            if(leftType == TYPE_ID){
                if(op == OP_EQ || op == OP_NEQ){
                    DataType idType;
                    bool known;
                    error = AnalyzeExpression(leftOperand, list, &idType, &known);
                    if(error){
                        return error;
                    }
                    if(idType == T_I32_N || idType == T_F64_N || idType == T_U8_N){
                        return 0;
                    }else{
                        return ERROR_TYPE_COMPABILITY;
                    }
                }
                return ERROR_TYPE_COMPABILITY;
            }else{
                return ERROR_TYPE_COMPABILITY;
            }
        }

        if(leftType == TYPE_STRING || rightType == TYPE_STRING){
            return ERROR_TYPE_COMPABILITY;
        }

        if(leftType == TYPE_ID){
            char *idLeft = GetId(leftOperand);
            TNode *leftTableNode = FindInSymlist(list, idLeft);
            if(leftTableNode == NULL){
                return ERROR_UNDEFINED;
            }

            SetIsUsed(leftTableNode, idLeft);

            NType leftNType;
            GetType(leftTableNode, idLeft, &leftNType);

            if(rightType == TYPE_ID){
                char *idRight = GetId(rightOperand);
                TNode *rightTableNode = FindInSymlist(list, idRight);
                if(rightTableNode == NULL){
                    return ERROR_UNDEFINED;
                }

                SetIsUsed(rightTableNode, idRight);

                NType rightNType;
                GetType(rightTableNode, idRight, &rightNType);

                if(op == OP_EQ || op == OP_NEQ){
                    if((leftNType == TYPE_I32 || leftNType == TYPE_I32_N) && (rightNType == TYPE_I32 || rightNType == TYPE_I32_N)){
                        return 0;
                    }else if((leftNType == TYPE_F64 || leftNType == TYPE_F64_N) && (rightNType == TYPE_F64 || rightNType == TYPE_F64_N)){
                        return 0;
                    }else{
                        return ERROR_TYPE_COMPABILITY;
                    }
                }else{
                    if(leftNType == TYPE_I32 && rightNType == TYPE_I32){
                        return 0;
                    }else if(leftNType == TYPE_F64 && rightNType == TYPE_F64){
                        return 0;
                    }else{
                        return ERROR_TYPE_COMPABILITY;
                    }
                }
            }else{
                DataType rightDataType;
                bool rightIsKnown;
                error = AnalyzeExpression(rightOperand, list, &rightDataType, &rightIsKnown);
                if(error){
                    return error;
                }

                bool leftIsConst;
                bool leftIsKnown;
                GetIsConstant(leftTableNode, idLeft, &leftIsConst);
                GetIsKnown(leftTableNode, idLeft, &leftIsKnown);

                if(op == OP_EQ || op == OP_NEQ){
                    if((leftNType == TYPE_I32 || leftNType == TYPE_I32_N) && rightDataType == T_I32){
                        return 0;
                    }else if((leftNType == TYPE_F64 || leftNType == TYPE_F64_N) && rightDataType == T_F64){
                        return 0;
                    }else if(leftIsConst && leftIsKnown && rightDataType == T_F64){
                        return 0;
                    }
                    return ERROR_TYPE_COMPABILITY;
                }else{
                    if(leftNType == TYPE_I32 && rightDataType == T_I32){
                        return 0;
                    }else if(leftNType == TYPE_F64 && (rightDataType == T_F64 || rightDataType == T_I32)){
                        return 0;
                    }else if(leftIsConst && leftIsKnown && rightDataType == T_F64){
                        return 0;
                    }
                    return ERROR_TYPE_COMPABILITY;
                }
            }
        }else if(rightType == TYPE_ID){
            char *idRight = GetId(rightOperand);
            TNode *rightTableNode = FindInSymlist(list, idRight);
            if(rightTableNode == NULL){
                return ERROR_UNDEFINED;
            }

            SetIsUsed(rightTableNode, idRight);

            NType rightNType;
            GetType(rightTableNode, idRight, &rightNType);

            if(leftType == TYPE_ID){
                char *idLeft = GetId(leftOperand);
                TNode *leftTableNode = FindInSymlist(list, idLeft);
                if(leftTableNode == NULL){
                    return ERROR_UNDEFINED;
                }

                SetIsUsed(leftTableNode, idLeft);

                NType leftNType;
                GetType(leftTableNode, idLeft, &leftNType);

                if(op == OP_EQ || op == OP_NEQ){
                    if((rightNType == TYPE_I32 || rightNType == TYPE_I32_N) && (leftNType == TYPE_I32 || leftNType == TYPE_I32_N)){
                        return 0;
                    }else if((rightNType == TYPE_F64 || rightNType == TYPE_F64_N) && (leftNType == TYPE_F64 || leftNType == TYPE_F64_N)){
                        return 0;
                    }else{
                        return ERROR_TYPE_COMPABILITY;
                    }
                }else{
                    if(rightNType == TYPE_I32 && leftNType == TYPE_I32){
                        return 0;
                    }else if(rightNType == TYPE_F64 && leftNType == TYPE_F64){
                        return 0;
                    }else{
                        return ERROR_TYPE_COMPABILITY;
                    }
                }
            }else{
                DataType leftDataType;
                bool leftIsKnown;
                error = AnalyzeExpression(leftOperand, list, &leftDataType, &leftIsKnown);
                if(error){
                    return error;
                }

                bool rightIsConst;
                bool rightIsKnown;
                GetIsConstant(rightTableNode, idRight, &rightIsConst);
                GetIsKnown(rightTableNode, idRight, &rightIsKnown);

                if(op == OP_EQ || op == OP_NEQ){
                    if((rightNType == TYPE_I32 || rightNType == TYPE_I32_N) && leftDataType == T_I32){
                        return 0;
                    }else if((rightNType == TYPE_F64 || rightNType == TYPE_F64_N) && leftDataType == T_F64){
                        return 0;
                    }else if(rightIsConst && rightIsKnown && leftDataType == T_F64){
                        return 0;
                    }
                    return ERROR_TYPE_COMPABILITY;
                }else{
                    if(rightNType == TYPE_I32 && leftDataType == T_I32){
                        return 0;
                    }else if(rightNType == TYPE_F64 && (leftDataType == T_F64 || leftDataType == T_I32)){
                        return 0;
                    }else if(rightIsConst && rightIsKnown && leftDataType == T_F64){
                        return 0;
                    }
                    return ERROR_TYPE_COMPABILITY;
                }
            }
        }else if(leftType == TYPE_VALUE_I32 || leftType == TYPE_VALUE_F64){
            DataType rightDataType;
            bool rightIsKnown;
            error = AnalyzeExpression(rightOperand, list, &rightDataType, &rightIsKnown);
            if(error){
                return error;
            }

            float f = GetFloatValue(leftOperand);
            if(f != floorf(f)){
                return ERROR_TYPE_COMPABILITY;
            }
            return 0;

        }else if(rightType == TYPE_VALUE_I32 || rightType == TYPE_VALUE_F64){
            DataType leftDataType;
            bool leftIsKnown;
            error = AnalyzeExpression(leftOperand, list, &leftDataType, &leftIsKnown);
            if(error){
                return error;
            }

            float f = GetFloatValue(rightOperand);
            if(f != floorf(f)){
                return ERROR_TYPE_COMPABILITY;
            }
            return 0;
        }

    }else if(type == TYPE_NULL){
        return ERROR_TYPE_COMPABILITY;
    }else if(type == TYPE_ID){
        char *id = GetId(node);
        TNode *idTableNode = FindInSymlist(list, id);
        if(idTableNode == NULL){
            return ERROR_UNDEFINED;
        }
        SetIsUsed(idTableNode, id);
        NType idType;
        GetType(idTableNode, id, &idType);
        if(idType == TYPE_I32_N || idType == TYPE_F64_N || idType == TYPE_U8_N){
            *hasNullId = true;
        }
        return 0;
    }else if(type == TYPE_OPERATOR){
        DataType dataType;
        bool known;
        return AnalyzeExpression(node, list, &dataType, &known);
    }else if(type == TYPE_VALUE_I32 || type == TYPE_VALUE_F64){
        return 0;
    }else{
        return ERROR_TYPE_COMPABILITY;
    }

    return 0;
}

/**
 * @brief analyzes return nodes, if they are the same as expected return type
 * @param node highest node from which we analyze
 * @param list SymList
 * @param returnType expected return type
 * @return 0 if successful, error code if not
 */
int AnalyzeReturnNode(ASTNode *node, SymList *list, DataType returnType){
    ASTNode *temp = GetNode(node);
    
    if(temp == NULL || returnType == T_VOID){
        if(temp == NULL && returnType == T_VOID){
            return 0;
        }
        return ERROR_BAD_RETURN;
    }
    int error = 0;

    ASTNodeType type = GetNodeType(temp);

    if(type == TYPE_ID){
        char *id = GetId(temp);
        TNode *symNode = FindInSymlist(list, id);
        if(symNode == NULL){
            return ERROR_UNDEFINED;
        }

        SetIsUsed(symNode, id);
        NType idType;
        GetType(symNode, id, &idType);

        if(idType == TYPE_I32){
            if(returnType == T_I32 || returnType == T_I32_N){
                return 0;
            }
            return ERROR_PARAMTERS;
        }else if(idType == TYPE_I32_N){
            if(returnType == T_I32_N){
                return 0;
            }
            return ERROR_PARAMTERS;
        }else if(idType == TYPE_F64){
            if(returnType == T_F64 || returnType == T_F64_N){
                return 0;
            }
            return ERROR_PARAMTERS;
        }else if(idType == TYPE_F64_N){
            if(returnType == T_F64_N){
                return 0;
            }
            return ERROR_PARAMTERS;
        }else if(idType == TYPE_U8){
            if(returnType == T_U8 || returnType == T_U8_N){
                return 0;
            }
            return ERROR_PARAMTERS;
        }else if(idType == TYPE_U8_N){
            if(returnType == T_U8_N){
                return 0;
            }
            return ERROR_PARAMTERS;
        }else{
            return ERROR_PARAMTERS;
        }
    }else if(type == TYPE_VALUE_I32){
        if(returnType == T_I32 || returnType == T_I32_N){
            return 0;
        }
        return ERROR_PARAMTERS;
    }else if(type == TYPE_VALUE_F64){
        if(returnType == T_F64 || returnType == T_F64_N){
            return 0;
        }
        return ERROR_PARAMTERS;
    }else if(type == TYPE_STRING){
        if(returnType == T_U8 || returnType == T_U8_N){
            return 0;
        }
        return ERROR_PARAMTERS;
    }else if(type == TYPE_OPERATOR){
        DataType expType;
        bool isKnown;
        error = AnalyzeExpression(temp, list, &expType, &isKnown);
        if(error){
            return error;
        }

        if(expType == T_I32){
            if(returnType == T_I32 || returnType == T_I32_N){
                return 0;
            }
            return ERROR_PARAMTERS;
        }

        if(expType == T_F64){
            if(returnType == T_F64 || returnType == T_F64_N){
                return 0;
            }
            return ERROR_PARAMTERS;
        }
    }else if(type == TYPE_NULL){
        if(returnType == T_I32_N || returnType == T_F64_N || returnType == T_U8_N){
            return 0;
        }
        return ERROR_PARAMTERS;
    }else{
        return ERROR_BAD_RETURN;
    }

    return 0;
}

/**
 * @brief checks if all variables created inside block were used
 * @param node symtable top
 * @return 0 if successful, error code if not
 */
int CheckVariablesUsed(TNode *node){
    if(node == NULL){
        return 0;
    }
    if(!GetIsUsed(node)){
        return ERROR_VARIABLE_NOT_USED;
    }
    int error = 0;
    error = CheckVariablesUsed(GetLeftNode(node));
    if(error){
        return error;
    }
    error = CheckVariablesUsed(GetRightNode(node));
    return error;
}

/**
 * @brief semantic analysis
 * @param root AST root
 * @return 0 if successful, error code if not
 */
int SemanticAnalysis(ASTNode *root){
    if(root == NULL){
        return INTERNAL_ERROR;
    }

    ASTStack *stack = CreateStackAST();
    ASTNode *currentNode = GetCode(root);
    TNode *symtable = NULL;
    int error;

    AddBuiltInFunctionsToSymtable(&symtable);

    // first pass that gets functions into symtable
    while(currentNode != NULL){
        ASTNode *temp = GetNode(currentNode);
        if(temp == NULL){
            break;
        }
        ASTNodeType type = GetNodeType(temp);
        if(type != TYPE_FUN_DECL){
            fprintf(stderr, "Error in semantincs: %d\n", error);
            FreeStackAST(stack);
            FreeTree(symtable);
            return ERROR_SEMANTIC_OTHERS;
        }
        error = AnalyzeFunDec(temp, &symtable);
        if(error){
            fprintf(stderr, "Error in semantincs: %d\n", error);
            FreeStackAST(stack);
            FreeTree(symtable);
            return error;
        }
        currentNode = GetCode(currentNode);
    }

    error = CheckForMainFunction(&symtable);
    if(error){
        fprintf(stderr, "Error in semantincs: %d\n", error);
        FreeStackAST(stack);
        FreeTree(symtable);
        return error;
    }

    DataType currentFunctionReturnType;
    SymList *symlist = CreateSymList();
    InsertTable(symlist, symtable);
    ASTNode *conNode;
    currentNode = GetCode(root);
    int level = 0;

    //second pass that analyzes semantics 
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
                    error = CheckVariablesUsed(symtable);
                    if(error){
                        fprintf(stderr, "Error in semantincs: %d\n", error);
                        FreeSemantics(stack, symlist);
                        return error;
                    }
                    DeleteTable(symlist); 
                    SymListNode *symListNode = GetLast(symlist);
                    symtable = GetTableNode(symListNode);
                    currentNode = PopAST(stack);
                }
                if(type == TYPE_FUN_DECL){
                    error = CheckVariablesUsed(symtable);
                    if(error){
                        fprintf(stderr, "Error in semantincs: %d\n", error);
                        FreeSemantics(stack, symlist);
                        return error;
                    }
                    DeleteTable(symlist);
                    SymListNode *symListNode = GetLast(symlist);
                    symtable = GetTableNode(symListNode);
                    currentNode = PopAST(stack);
                }
                if(type == TYPE_WHILE_CLOSED){
                    error = CheckVariablesUsed(symtable);
                    if(error){
                        fprintf(stderr, "Error in semantincs: %d\n", error);
                        FreeSemantics(stack, symlist);
                        return error;
                    }
                    DeleteTable(symlist);
                    SymListNode *symListNode = GetLast(symlist);
                    symtable = GetTableNode(symListNode);
                    currentNode = PopAST(stack);
                }
            }
            currentNode = GetCode(currentNode);
            continue;
        }

        ASTNodeType type = GetNodeType(currentNode);
        bool hasNullId;

        switch(type){
            case TYPE_CODE:
                PushAST(stack, currentNode);
                currentNode = GetNode(currentNode);
                level++;
                continue;
            case TYPE_FUN_DECL:
                if(level == 1){
                    symtable = GetFunctionSymtable(currentNode, &currentFunctionReturnType);
                    InsertTable(symlist, symtable);
                    PushAST(stack, currentNode);
                    currentNode = GetNode(currentNode);
                }else{
                    fprintf(stderr, "Error in semantincs: %d\n", error);
                    FreeSemantics(stack, symlist);
                    return ERROR_SEMANTIC_OTHERS;
                }
                break;
            case TYPE_RETURN:
                error = AnalyzeReturnNode(currentNode, symlist, currentFunctionReturnType);
                if(error){
                    fprintf(stderr, "Error in semantincs: %d\n", error);
                    FreeSemantics(stack, symlist);
                    return error;
                }
                level--;
                currentNode = PopAST(stack);
                currentNode = GetCode(currentNode);
                break;
            case TYPE_IF_ELSE:
                level++;
                conNode = GetConditionNode(currentNode);
                error = AnalyzeCondition(conNode, symlist, &hasNullId);
                if(error){
                    fprintf(stderr, "Error in semantincs: %d\n", error);
                    FreeSemantics(stack, symlist);
                    return error;
                }
                symtable = GetBlockSymtable(currentNode, symlist, &error);
                if(error){
                    fprintf(stderr, "Error in semantincs: %d\n", error);
                    FreeSemantics(stack, symlist);
                    return error;
                }
                if(symtable != NULL && !hasNullId){
                    fprintf(stderr, "Error in semantincs: %d\n", error);
                    FreeSemantics(stack, symlist);
                    return ERROR_TYPE_COMPABILITY;
                }
                InsertTable(symlist, symtable);
                PushAST(stack, GetElseNode(currentNode));
                currentNode = GetIfNode(currentNode);
                PushAST(stack,currentNode);
                break;
            case TYPE_WHILE_CLOSED:
                conNode = GetConditionNode(currentNode);
                error = AnalyzeCondition(conNode, symlist, &hasNullId);
                if(error){
                    fprintf(stderr, "Error in semantincs: %d\n", error);
                    FreeSemantics(stack, symlist);
                    return error;
                }
                symtable = GetBlockSymtable(currentNode, symlist, &error);
                if(error){
                    fprintf(stderr, "Error in semantincs: %d\n", error);
                    FreeSemantics(stack, symlist);
                    return error;
                }
                if(symtable != NULL && !hasNullId){
                    fprintf(stderr, "Error in semantincs: %d\n", error);
                    FreeSemantics(stack, symlist);
                    return ERROR_TYPE_COMPABILITY;
                }
                InsertTable(symlist, symtable);
                PushAST(stack, currentNode);
                currentNode = GetNode(currentNode);
                break;
            case TYPE_VAR_DECL:
            case TYPE_CON_DECL:
                error = AnalyzeVariableDeclaration(currentNode, symlist);
                if(error){
                    fprintf(stderr, "Error in semantincs: %d\n", error);
                    FreeSemantics(stack, symlist);
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
                    fprintf(stderr, "Error in semantincs: %d\n", error);
                    FreeSemantics(stack, symlist);
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
                    fprintf(stderr, "Error in semantincs: %d\n", error);
                    FreeSemantics(stack, symlist);
                    return error;
                }
                if(funReturnType != TYPE_VOID){
                    fprintf(stderr, "Error in semantincs: %d\n", error);
                    FreeSemantics(stack, symlist);
                    return ERROR_PARAMTERS;
                }
                currentNode = PopAST(stack);
                currentNode = GetCode(currentNode);
                level--;
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
    FreeSemantics(stack, symlist);

    return 0;
}

