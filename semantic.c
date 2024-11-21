#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

void FreeSemantics(ASTStack *stack, SymList *list){
    FreeStackAST(stack);
    FreeSymlist(list);
}

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

int AnalyzeFunDec(ASTNode *node, TNode **symtable){
    if(node == NULL || node->type != TYPE_FUN_DECL){
        return 10;
    }
    ASTNode *temp = GetIdNode(node);
    char *idFunction = GetId(temp);

    TNode *exists = SearchNode(*symtable, idFunction);
    if(exists){
        return 5;
    }
    *symtable = InsertNode(*symtable,idFunction);
    SetType(*symtable, idFunction, TYPE_FUNCTION);
    DataType type = GetDataType(temp);

    if(type != T_VOID){
        int ret = LookForReturnNodes(GetNode(node));
        if(!ret){
            return 6;
        }
    }

    SetFunctionReturnType(*symtable, idFunction, DataTypeToNType(type));
    temp = GetParamNode(temp);
    while(temp != NULL){
        char *id = GetId(temp);
        type = GetDataType(temp);
        SetParameter(*symtable, idFunction, DataTypeToNType(type));
        temp = GetParamNode(temp);
    }

    return 0;

}

int CheckForMainFunction(TNode **symtable){
    NType type;
    int error = GetType(*symtable, "main", &type);
    if(error == -1 || type != TYPE_FUNCTION){
        return 3;
    }
    int count;
    error = GetParameterCount(*symtable, "main", &count);
    if(error == -1 || count != 0){
        return 4;
    }
    error = GetFunctionReturnType(*symtable, "main", &type);
    if(error == -1 || type != TYPE_VOID){
        return 4;
    }
    return 0;
}

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
        SymListNode *tempNode = GetLast(list);
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
                newType == TYPE_I32;
            }else if(tableType == TYPE_F64_N){
                newType == TYPE_F64;
            }else{
                newType == TYPE_U8;
            }
            id = GetId(idNoNullNode);
            newSymtable = InsertNode(newSymtable, id);
            SetType(newSymtable, id, newType);
        }
        *error = 7;
        return newSymtable;
    }
    return newSymtable;
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

        if(expTypeL == T_I32){
            if(expTypeR == T_I32){
                *expType = T_I32;
                return 0;
            }else if(expTypeR == T_F64){
                *expType = T_F64;
                return 0;
            }else{
                return 7;
            }
        }else if(expTypeL == T_F64){
            if(expTypeR == T_I32){
                *expType = T_F64;
                return 0;
            }else if(expTypeR == T_F64){
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
        if(idType == TYPE_I32){
            *expType = T_I32;
            return 0;
        }else if(idType == TYPE_F64){
            *expType = T_F64;
            return 0;
        }else{
            return 7;
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
    }else{
        return 7;
    }
    return error;
}

int AnalyzeFunctionCall(ASTNode *node, SymList *list, NType *type){
    ASTNode *idNode = GetIdNode(node);
    char *id = GetId(idNode);
    TNode *tableNode = FindInSymlist(list, id);
    if(tableNode == NULL){
        return 3;
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
                return 4;
            }
        }else if(nodeType == TYPE_STRING){
            if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.string")){
                return 0;
            }else{
                return 4;
            }
        }else if(nodeType == TYPE_VALUE_I32){
            if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.i2f")){
                return 0;
            }else{
                return 4;
            }
        }else if(nodeType == TYPE_VALUE_F64){
            if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.f2i")){
                return 0;
            }else{
                return 4;
            }
        }else if(nodeType == TYPE_ID){
            char *argId = GetId(special);
            TNode *argumentNode = FindInSymlist(list, argId);
            if(argumentNode == NULL){
                return 3;
            }
            SetIsUsed(argumentNode, argId);
            NType argtype;
            GetType(argumentNode, argId, &argtype);
            if(argtype == TYPE_I32){
                if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.i2f")){
                    return 0;
                }else{
                    return 4;
                }
            }else if(argtype == TYPE_I32_N){
                if(!strcmp(id, "ifj.write")){
                    return 0;
                }else{
                    return 4;
                }
            }else if(argtype == TYPE_F64){
                if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.f2i")){
                    return 0;
                }else{
                    return 4;
                }
            }else if(argtype == TYPE_F64_N){
                if(!strcmp(id, "ifj.write")){
                    return 0;
                }else{
                    return 4;
                }
            }else if(argtype == TYPE_U8){
                if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.string")){
                    return 0;
                }else{
                    return 4;
                }
            }else if(argtype == TYPE_U8_N){
                if(!strcmp(id, "ifj.write")){
                    return 0;
                }else{
                    return 4;
                }
            }
        }else if(nodeType == TYPE_OPERATOR){
            DataType expType;
            bool isKnown;
            int error = AnalyzeExpression(special, list, &expType, &isKnown);
            if(error){
                return error;
            }

            if(expType == T_I32){
                if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.i2f")){
                    return 0;
                }else{
                    return 4;
                }
            }else{
                if(!strcmp(id, "ifj.write") || !strcmp(id, "ifj.f2i")){
                    return 0;
                }else{
                    return 4;
                }
            }
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

            return 4;
        }else{
            return 4;
        }
    }

    if(paramCount != argCount){
        return 4;
    }

    return 0;
}

int AnalyzeVariableDeclaration(ASTNode *node, SymList *list){
    ASTNode *idNode = GetIdNode(node);
    char *id = GetId(idNode);

    if(!strcmp(id, "_")){
        return 10;
    }

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
            return 7;
        }

    }else if(astType == TYPE_ID || astType == TYPE_FUN_CALL){
        NType rightType;
        if(astType == TYPE_ID){
            char *idRight = GetId(temp);
            TNode *idRightTableNode = FindInSymlist(list, idRight);
            if(idRightTableNode == NULL){
                return 3;
            }
            SetIsUsed(idRightTableNode, idRight);
            GetType(idRightTableNode, idRight, &rightType);
            GetIsKnown(idRightTableNode, idRight, &isKnown);
        }else{
            error = AnalyzeFunctionCall(temp, list, &rightType);
            if(error){
                return error;
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
                return 7;
            }
        }else if(type == T_I32_N){
            if(rightType == TYPE_I32 || rightType == TYPE_I32_N){
                symtable = InsertNode(symtable, id);
                SetType(symtable, id, TYPE_I32_N);
            }else{
                return 7;
            }
        }else if(type == T_F64){
            if(rightType == TYPE_F64){
                symtable = InsertNode(symtable, id);
                SetType(symtable, id, rightType);
            }else{
                return 7;
            }
        }else if(type == T_F64_N){
            if(rightType == TYPE_F64 || rightType == TYPE_F64_N){
                symtable = InsertNode(symtable, id);
                SetType(symtable, id, TYPE_F64_N);
            }else{
                return 7;
            }
        }else if(type == T_U8){
            if(rightType == TYPE_U8){
                symtable = InsertNode(symtable, id);
                SetType(symtable, id, rightType);
            }else{
                return 7;
            }
        }else if(type == T_U8_N){
            if(rightType == TYPE_U8 || rightType == TYPE_U8_N){
                symtable = InsertNode(symtable, id);
                SetType(symtable, id, TYPE_U8_N);
            }else{
                return 7;
            }
        }
    }else if(astType == TYPE_FUN_CALL){
        NType rightType;
        AnalyzeFunctionCall(temp, list, &rightType);
    }else{
        return 7;
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
            return 5;
        }

        if(tableNode == NULL){
            return 3;
        }
    }

    ASTNode *temp = GetNode(node);
    if(temp == NULL){
        return -1;
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
            return 7;
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
                return 7;
            }
        }else if(type == TYPE_F64 || type == TYPE_F64_N){
            if(expType == T_F64){
                return 0;
            }else{
                return 7;
            }
        }
    }else if(astType == TYPE_ID || astType == TYPE_FUN_CALL){
        NType rightType;
        if(astType == TYPE_ID){
            char *idRight = GetId(temp);
            TNode *idRightTableNode = FindInSymlist(list, idRight);
            if(idRightTableNode == NULL){
                return 3;
            }
            SetIsUsed(idRightTableNode, idRight);
            GetType(idRightTableNode, idRight, &rightType);
            GetIsKnown(idRightTableNode, idRight, &isKnown);
        }else{
            error = AnalyzeFunctionCall(temp, list, &rightType);
            if(error){
                return error;
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
            return 7;
        }
    }else{
        return 7;
    }
    
   return 0;
}

int AnalyzeCondition(ASTNode *node, SymList *list, bool *hasNullId){
    if(node == NULL){
        //error
    }

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
                return 7;
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
                        return 7;
                    }
                }
                return 7;
            }else{
                return 7;
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
                        return 7;
                    }
                }
                return 7;
            }else{
                return 7;
            }
        }

        if(leftType == TYPE_STRING || rightType == TYPE_STRING){
            return 7;
        }

        if(leftType == TYPE_ID){
            char *idLeft = GetId(leftOperand);
            TNode *leftTableNode = FindInSymlist(list, idLeft);
            if(leftTableNode == NULL){
                return 3;
            }

            SetIsUsed(leftTableNode, idLeft);

            NType leftNType;
            GetType(leftTableNode, idLeft, &leftNType);

            if(rightType == TYPE_ID){
                char *idRight = GetId(rightOperand);
                TNode *rightTableNode = FindInSymlist(list, idRight);
                if(rightTableNode == NULL){
                    return 3;
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
                        return 7;
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
                    return 7;
                }else{
                    if(leftNType == TYPE_I32 && rightDataType == T_I32){
                        return 0;
                    }else if(leftNType == TYPE_F64 && rightDataType == T_F64){
                        return 0;
                    }else if(leftIsConst && leftIsKnown && rightDataType == T_F64){
                        return 0;
                    }
                    return 7;
                }
            }
        }else if(rightType == TYPE_ID){
            char *idRight = GetId(rightOperand);
            TNode *rightTableNode = FindInSymlist(list, idRight);
            if(rightTableNode == NULL){
                return 3;
            }

            SetIsUsed(rightTableNode, idRight);

            NType rightNType;
            GetType(rightTableNode, idRight, &rightNType);

            if(leftType == TYPE_ID){
                char *idLeft = GetId(leftOperand);
                TNode *leftTableNode = FindInSymlist(list, idLeft);
                if(leftTableNode == NULL){
                    return 3;
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
                        return 7;
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
                    return 7;
                }else{
                    if(rightNType == TYPE_I32 && leftDataType == T_I32){
                        return 0;
                    }else if(rightNType == TYPE_F64 && leftDataType == T_F64){
                        return 0;
                    }else if(rightIsConst && rightIsKnown && leftDataType == T_F64){
                        return 0;
                    }
                    return 7;
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
                return 7;
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
                return 7;
            }
            return 0;
        }

    }else if(type == TYPE_NULL){
        return 7;
    }else if(type == TYPE_ID){
        char *id = GetId(node);
        TNode *idTableNode = FindInSymlist(list, id);
        if(idTableNode == NULL){
            return 3;
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
        return 7;
    }

}

int AnalyzeReturnNode(ASTNode *node, SymList *list, DataType returnType){
    ASTNode *temp = GetNode(node);
    
    if(temp == NULL || returnType == T_VOID){
        if(temp == NULL && returnType == T_VOID){
            return 0;
        }
        return 6;
    }
    int error = 0;

    ASTNodeType type = GetNodeType(temp);

    if(type == TYPE_ID){
        char *id = GetId(temp);
        TNode *symNode = FindInSymlist(list, id);
        if(symNode == NULL){
            return 3;
        }

        SetIsUsed(symNode, id);
        NType idType;
        GetType(symNode, id, &idType);

        if(idType == TYPE_I32){
            if(returnType == T_I32 || returnType == T_I32_N){
                return 0;
            }
            return 4;
        }else if(idType == TYPE_I32_N){
            if(returnType == T_I32_N){
                return 0;
            }
            return 4;
        }else if(idType == TYPE_F64){
            if(returnType == T_F64 || returnType == T_F64_N){
                return 0;
            }
            return 4;
        }else if(idType == TYPE_F64_N){
            if(returnType == T_F64_N){
                return 0;
            }
            return 4;
        }else if(idType == TYPE_U8){
            if(returnType == T_U8 || returnType == T_U8_N){
                return 0;
            }
            return 4;
        }else if(idType == TYPE_U8_N){
            if(returnType == T_U8_N){
                return 0;
            }
            return 4;
        }else{
            return 4;
        }
    }else if(type == TYPE_VALUE_I32){
        if(returnType == T_I32 || returnType == T_I32_N){
            return 0;
        }
        return 4;
    }else if(type == TYPE_VALUE_F64){
        if(returnType == T_F64 || returnType == T_F64_N){
            return 0;
        }
        return 4;
    }else if(type == TYPE_STRING){
        if(returnType == T_U8 || returnType == T_U8_N){
            return 0;
        }
        return 4;
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
            return 4;
        }

        if(expType == T_F64){
            if(returnType == T_F64 || returnType == T_F64_N){
                return 0;
            }
            return 4;
        }
    }else if(type == TYPE_NULL){
        if(returnType == T_I32_N || returnType == T_F64_N || returnType == T_U8_N){
            return 0;
        }
        return 4;
    }else{
        return 6;
    }
}

int CheckVariablesUsed(TNode *node){
    if(node == NULL){
        return 0;
    }
    if(!GetIsUsed(node)){
        return 9;
    }
    int error = 0;
    error = CheckVariablesUsed(GetLeftNode(node));
    if(error){
        return error;
    }
    error = CheckVariablesUsed(GetRightNode(node));
    return error;
}

int SemanticAnalysis(ASTNode *root){
    if(root == NULL){
        return 99;
    }

    ASTStack *stack = CreateStackAST();
    ASTNode *currentNode = GetCode(root);
    TNode *symtable = NULL;
    int error;

    AddBuiltInFunctionsToSymtable(&symtable);

    while(currentNode != NULL){
        ASTNode *temp = GetNode(currentNode);
        if(temp == NULL){
            break;
        }
        ASTNodeType type = GetNodeType(temp);
        if(type != TYPE_FUN_DECL){
            FreeStackAST(stack);
            FreeTree(symtable);
            return 10;
        }
        error = AnalyzeFunDec(temp, &symtable);
        if(error){
            FreeStackAST(stack);
            FreeTree(symtable);
            return error;
        }
        currentNode = GetCode(currentNode);
    }

    error = CheckForMainFunction(&symtable);
    if(error){
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
                    FreeSemantics(stack, symlist);
                    return 10;
                }
                break;
            case TYPE_RETURN:
                error = AnalyzeReturnNode(currentNode, symlist, currentFunctionReturnType);
                if(error){
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
                    FreeSemantics(stack, symlist);
                    return error;
                }
                symtable = GetBlockSymtable(currentNode, symlist, &error);
                if(error){
                    FreeSemantics(stack, symlist);
                    return error;
                }
                if(symtable != NULL && !hasNullId){
                    FreeSemantics(stack, symlist);
                    return 7;
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
                    FreeSemantics(stack, symlist);
                    return error;
                }
                symtable = GetBlockSymtable(currentNode, symlist, &error);
                if(error){
                    FreeSemantics(stack, symlist);
                    return error;
                }
                if(symtable != NULL && !hasNullId){
                    FreeSemantics(stack, symlist);
                    return 7;
                }
                InsertTable(symlist, symtable);
                PushAST(stack, currentNode);
                currentNode = GetNode(currentNode);
                break;
            case TYPE_VAR_DECL:
            case TYPE_CON_DECL:
                error = AnalyzeVariableDeclaration(currentNode, symlist);
                if(error){
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
                    FreeSemantics(stack, symlist);
                    return error;
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

ASTNode* createAST(){
    ASTNode *root = CreateAST();

    ASTNode *temp2 = CreateCodeNode(root);
    temp2 = CreateFunDeclNode(temp2);
    CreateIdNode(temp2, "add");
    CreateTypeNode(temp2, T_I32);
    ASTNode *in = CreateCodeNode(temp2);
    ASTNode *temp3 = CreateWhileNode(in);
    temp3->type = TYPE_WHILE_CLOSED;
    temp3->left->right = CreateAstNode(TYPE_VALUE_I32);

    temp3->right = CreateAstNode(TYPE_CODE);
    temp3->right->right = CreateAstNode(TYPE_RETURN);
    temp3->right->right->right = CreateAstNode(TYPE_VALUE_I32);
    //ASTNode *temp3 = CreateIfElseNode(in);
    /*temp3->left->left = (ASTNode *)malloc(sizeof(ASTNode));
    temp3->left->left->type = TYPE_ID;
    temp3->left->left->data.str = strdup("aaaa");*/
    /*temp3->left->right = CreateAstNode(TYPE_VALUE_I32);
    ASTNode *temp4 = CreateIfNode(temp3);
    ASTNode *temp5 = CreateElseNode(temp3);
    temp4->left->type = TYPE_IF_CLOSED;
    temp5->right->right->type = TYPE_ELSE_CLOSED;

    temp4->left->left = CreateAstNode(TYPE_CODE);
    temp4->left->left->right = CreateAstNode(TYPE_RETURN);
    temp4->left->left->right->right = CreateAstNode(TYPE_VALUE_I32);

    temp4->right->left = CreateAstNode(TYPE_CODE);
    //temp4->right->left->right = CreateAstNode(TYPE_RETURN);
    //temp4->right->left->right->right = CreateAstNode(TYPE_VALUE_I32);*/
    
    /*in = CreateCodeNode(in);
    CreateReturnNode(in);
    in->right->right = CreateAstNode(TYPE_VALUE_I32);*/


    temp2 = CreateCodeNode(root);
    temp2 = CreateFunDeclNode(temp2);
    CreateIdNode(temp2, "main");
    CreateTypeNode(temp2, T_VOID);


    temp2 = CreateCodeNode(temp2);
    ASTNode *funca = CreateFunCallNode(temp2);
    CreateIdNode(funca, "add");

    /*temp2 = CreateCodeNode(temp2);
    ASTNode *var2 = CreateVarDeclNode(temp2);
    CreateIdNode(var2, "_");
    CreateTypeNode(var2, T_I32);
    var2->right = CreateAstNode(TYPE_VALUE_I32);*/

   /* temp2 = CreateCodeNode(temp2);
    temp2->right = CreateAstNode(TYPE_ASSIGNMENT);
    CreateIdNode(temp2->right, "_");
    temp2->right->right = CreateAstNode(TYPE_ID);
    temp2->right->right->data.str = "a";*/

    /*ASTNode *temp = CreateCodeNode(root);
    temp = CreateFunDeclNode(temp);
    CreateIdNode(temp, "maian");
    CreateParamNode(temp, "a");
    CreateTypeNode(temp, T_F64);
    CreateParamNode(temp, "b");
    CreateTypeNode(temp, T_F64);
    CreateParamNode(temp, "c");
    CreateTypeNode(temp, T_F64);
    CreateTypeNode(temp, T_I32);

    temp3 = CreateCodeNode(temp);
    ret = CreateReturnNode(temp3);*/

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

    return root;
}


/*int main(){
    ASTNode *root = createAST();
    int a = SemanticAnalysis(root);
    printf("%d", a);
    return 0;
}*/