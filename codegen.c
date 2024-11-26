#include "codegen.h"

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
    printf("PUSHFRAME\n");
}

// Funkce
void CreateFunctionCall(char* function_id) {
    printf("CALL $%s\n", function_id);
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
            CreateVariable(GetId(node->left));
            level--;
            break;
        case TYPE_CON_DECL:
            CreateVariable(GetId(node->left));
            level--;
            break;
        case TYPE_FUN_CALL:
            ASTNode *paramsNode = GetNode(node);
            // TOTO AK NENI PARAMETER VARIABLE AK JE VARIABLE ASI TO BUDE INAK
            while (paramsNode != NULL)
            {
                CreateNonVariableParams(params);
                CreateNonVariableParamsData(*paramsNode, params);
                params++;
                paramsNode = GetArgNode(paramsNode);
            }
            CreateFunctionCall(GetId(node->left));
            
            level--;
            break;
        case TYPE_RETURN:
            level--;
            break;
        case TYPE_ASSIGNMENT:
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

// void appendFunctionParam(char* param_id)
// { 
//     printf("DEFVAR LF@%s\n",param_id);	
// }

// // DataTypes
// void dataType(char* id,DataType type, char* input)
// {
//     printf("WRITE GF@%s\n", input);

//     switch (type)
//     {
//         case T_F64:
//         case T_F64_N:
//             printf("READ LF@$%s int\n", id);
//             break;
//         case T_I32:
//         case T_I32_N:
//             printf("READ LF@$%s float\n", id);
//             break;
//         case T_U8:
//         case T_U8_N:
//             printf("READ LF@$%s string\n", id);
//             break;
//     }
// }

// // Var decleration

// void varDeclaration(char *var_id, DataType type, char* input) 
// {
//     printf("DEFVAR LF@$%s\n", var_id);
// }

// void assignment(char *assig_id)
// {
//     printf("LABEL $%s\n", assig_id);
// }

// // header, jump on main
// void header()
// {
//     printf(".IFJcode24\n");

//     printf("JUMP $$main");
// }

// void expression(char *id, Operator operator)
// {
//     switch(operator)
//     {
//         case OP_ADD:
//             printf("ADD LF@$%s TF@%value1 TF@%value2", id);
//             break;
//         case OP_SUB:
//             printf("SUB LF@$%s TF@%value1 TF@%value2", id);
//             break;
//         case OP_MUL:
//             printf("MUL LF@$%s TF@%value1 TF@%value2", id);
//             break;
//         case OP_DIV:
//             printf("DIV LF@$%s TF@%value1 TF@%value2", id);
//             break;
//     }
// }

// void relationalContidion(char *id, Operator operator)
// {
//     switch(operator)
//     {
//         case OP_EQ:
//             printf("EQ LF@$%s TF@%value1 TF@%value2", id);
//             break;
//         case OP_GREATER:
//             printf("GT LF@$%s TF@%value1 TF@%value2", id);
//             break;
//         case OP_LESS:
//             printf("LT LF@$%s TF@%value1 TF@%value2", id);
//             break;
//         case OP_GE:

//             break;
//         case OP_NEQ:
//             break;
//         case OP_LE:
//             break;
//     }
// }


