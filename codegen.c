#include "codegen.h"
// Konec printu

void CreateHeader()
{
    printf(".IFJcode24\n");
    printf("JUMP $main\n\n");
}

void CreateMain()
{
    printf("LABEL $main\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void CreateFunction(char *function_id)
{
    printf("LABEL $%s\n", function_id);
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

// Funkce
/*void CreateFunctionCall(char* function_id) {
    printf("CALL $%s\n", function_id);
}*/

void CreateFunctionCall(ASTNode *node) {
    //printf("CALL $%s\n", function_id);
    char *functionId = GetId(node->left);
    if(!strcmp(functionId, "ifj.write")){
        ASTNodeType type = GetNodeType(node->right);
        if(type == TYPE_ARGUMENT){
            char *id = GetId(node->right);
            printf("WRITE LF@%s\n", id);
        }else if(type == TYPE_NULL){
            printf("WRITE nil@nil\n");
        }else if(type == TYPE_VALUE_I32){
            int value = node->right->data.i32;
            printf("WRITE int@%d\n",value);
        }else if(type == TYPE_VALUE_F64){
            float value = node->right->data.f64;
            printf("WRITE float@%f\n",value);
        }else if(type == TYPE_STRING){
            char *string = node->right->data.str;
            printf("WRITE string@%s\n",string);
        }
    }else{
        printf("CALL $%s\n", functionId);
    }
}

void CreateVariable(char* var_id)
{
    printf("DEFVAR LF@%s\n", var_id);
}

void CreateNonVariableParams(int param)
{
    printf("DEFVAR TF@param%i\n", param);
}

void CreateNonVariableParamsData(ASTNode data,int param)
{
    if(data.type == TYPE_VALUE_I32)
    {
        printf("MOVE TF@param%i int@%i\n", param, data.data.i32);
    }
    else if(data.type == TYPE_VALUE_F64)
    {
        printf("MOVE TF@param%i float@%f\n", param, data.data.f64);
    }
    else if(data.type == TYPE_STRING)
    {
        printf("MOVE TF@param%i string@%s\n", param, data.data.str);
    }
    else if(data.type == TYPE_ARGUMENT)
    {
        printf("MOVE TF@param%i LF@%s\n", param, data.data.str);
    }
    else
    {
        printf("MOVE TF@param%i nil@nil\n", param);
    }
}

void CreateExpression(ASTNode *node){
    ASTNodeType type = GetNodeType(node);
    if(type == TYPE_VALUE_I32){
        int value = node->data.i32;
        printf("PUSHS int@%d\n", value);
    }else if(type == TYPE_VALUE_F64){
        float value = node->data.f64;
        printf("PUSHS float@%f\n", value);
    }else if(type == TYPE_ID){
        char *id = GetId(node);
        printf("PUSHS LF@%s\n", id);
    }else if(type == TYPE_FUN_CALL){

    }else if(type == TYPE_OPERATOR){
        CreateExpression(node->left);
        CreateExpression(node->right);
        Operator op = GetOperator(node);
        if(op == OP_ADD){
            printf("ADDS\n");
        }else if(op == OP_SUB){
            printf("SUBS\n");
        }else if(op == OP_MUL){
            printf("MULS\n");
        }else if(op == OP_DIV){
            printf("DIVS\n");
        }
    }
}

int TraverseASTCodeGen(ASTNode *node){
    int params = 1;
    int error = 0;
    static int level = 0;
    if(node == NULL){
        level--;
        return error;
    }

    ASTNodeType type = GetNodeType(node);

    switch(type){
        case TYPE_PROGRAM:
            CreateHeader();
            error = TraverseASTCodeGen(GetCode(node));
            break;
        case TYPE_CODE:
            level++;
            error = TraverseASTCodeGen(GetNode(node));
            error = TraverseASTCodeGen(GetCode(node));
            break;
        case TYPE_FUN_DECL:
            if(!strcmp(GetId(node->left), "main"))
            {
                CreateMain();
            }
            else
            {
                CreateFunction(GetId(node->left));
            }
            error = TraverseASTCodeGen(GetNode(node));
            break;
        case TYPE_VAR_DECL:
        case TYPE_CON_DECL:
            CreateVariable(GetId(node->left));
            /*if (node->right->type == TYPE_OPERATOR)
            {
                if (node->right->left->type == TYPE_VALUE_I32)
                {
                    ExpressionsOperatorInt(GetId(node->left), node->right);
                }
                else
                {
                    ExpressionsOperatorFloat(GetId(node->left), node->right);
                }
            }
            else
            {
                ExpressionsOperand(GetId(node->left), node->right);
            }*/
            CreateExpression(node->right);
            printf("POPS LF@%s\n",GetId(node->left));
            level--;
            break;
        /*case TYPE_CON_DECL:
            CreateVariable(GetId(node->left));
            ExpressionsOperand(GetId(node->left), node->right);
            level--;
            break;*/
        case TYPE_FUN_CALL:
            /*ASTNode *paramsNode = GetNode(node);
            // TOTO AK NENI PARAMETER VARIABLE AK JE VARIABLE ASI TO BUDE INAK
            while (paramsNode != NULL)
            {
                CreateNonVariableParams(params);
                CreateNonVariableParamsData(*paramsNode, params);
                params++;
                paramsNode = GetArgNode(paramsNode);
            }
            CreateFunctionCall(GetId(node->left));*/
            CreateFunctionCall(node);
            level--;
            break;
        case TYPE_RETURN:
            level--;
            break;
        case TYPE_ASSIGNMENT:
            CreateExpression(node->right);
            printf("POPS LF@%s\n",GetId(node->left));
            /*if (node->right->type == TYPE_OPERATOR)
            {
                if (node->right->left->type == TYPE_VALUE_I32)
                {
                    ExpressionsOperatorInt(GetId(node->left), node->right);
                }
                else
                {
                    ExpressionsOperatorFloat(GetId(node->left), node->right);
                }
            }
            else
            {
                ExpressionsOperand(GetId(node->left), node->right);
            }*/
            level--;
            break;
        case TYPE_IF_ELSE:
            error = TraverseASTCodeGen(node->left->right);
            error = TraverseASTCodeGen(node->left->left);
            break;
        case TYPE_WHILE_CLOSED:
            error = TraverseASTCodeGen(GetNode(node));
            break;
        case TYPE_IF_CLOSED:
            error = TraverseASTCodeGen(GetCode(node));
            break;
        case TYPE_ELSE_CLOSED:
            error = TraverseASTCodeGen(GetCode(node));
            break;
        default:
            break;
    }

    return error;
}
void ExpressionsOperand(char* id, ASTNode* Input)
{
    if (Input->type == TYPE_VALUE_I32)
    {
        printf("MOVE LF@%s int@%d\n", id, GetIntValue(Input));
    }
    else if (Input->type == TYPE_VALUE_F64)
    {
        printf("MOVE LF@%s float@%f\n", id, GetFloatValue(Input));
    }
    else
    {
        printf("MOVE LF@%s string@%s\n", id, GetId(Input));
    }
}

void ExpressionsOperatorInt(char* id, ASTNode* Input)
{
    Operator operator = GetOperator(Input);
    int value1 = GetIntValue(Input->left);
    int value2 = GetIntValue(Input->right);
    switch (operator)
    {
    case OP_ADD:
        printf("ADD LF@%s int@%d int@%d\n", id, value1, value2);
        break;
    case OP_SUB:
        printf("SUB LF@%s int@%d int@%d\n", id, value1, value2);
        break;
    case OP_MUL:
        printf("MUL LF@%s int@%d int@%d\n", id, value1, value2);
        break;
    case OP_DIV:
        printf("DIV LF@%s int@%d int@%d\n", id, value1, value2);
        break;
    case OP_EQ:
        printf("EQ LF@%s int@%d int@%d\n", id, value1, value2);
        break;
    case OP_GREATER:
        printf("GT LF@%s int@%d int@%d\n", id, value1, value2);
        break;
    case OP_LESS:
        printf("LT LF@%s int@%d int@%d\n", id, value1, value2);
        break;
    default:
        break;
    }
}

void ExpressionsOperatorFloat(char* id, ASTNode* Input)
{
    Operator operator = GetOperator(Input);
    float value1 = GetFloatValue(Input->left);
    float value2 = GetFloatValue(Input->right);
    switch (operator)
    {
    case OP_ADD:
        printf("ADD LF@%s float@%f float@%f\n", id, value1, value2);
        break;
    case OP_SUB:
        printf("SUB LF@%s float@%f float@%f\n", id, value1, value2);
        break;
    case OP_MUL:
        printf("MUL LF@%s float@%f float@%f\n", id, value1, value2);
        break;
    case OP_DIV:
        printf("DIV LF@%s float@%f float@%f\n", id, value1, value2);
        break;
    case OP_EQ:
        printf("EQ LF@%s int@%f int@%f\n", id, value1, value2);
        break;
    case OP_GREATER:
        printf("GT LF@%s int@%f int@%f\n", id, value1, value2);
        break;
    case OP_LESS:
        printf("LT LF@%s int@%f int@%f\n", id, value1, value2);
        break;
    default:
        break;
    }
}

