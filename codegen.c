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

int TraverseASTCodeGen(ASTNode *node){
    int error = 0;
    static int level = 0;
    if(node == NULL){
        level--;
        return error;
    }

    // for(int i = 0; i < level; i++){
    //     printf("    ");
    // }

    ASTNodeType type = GetNodeType(node);

    switch(type){
        case TYPE_PROGRAM:
            CreateHeader();
            //printf("PROGRAM\n");
            error = TraverseASTCodeGen(GetCode(node));
            break;
        case TYPE_CODE:
            level++;
            error = TraverseASTCodeGen(GetNode(node));
            error = TraverseASTCodeGen(GetCode(node));
            break;
        case TYPE_FUN_DECL:
            //printf("FUNCTION DECLARATION\n");
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
            //printf("VARIABLE DECLARATION\n");
            level--;
            break;
        case TYPE_CON_DECL:
            //printf("CONSTANT DECLARATION\n");
            level--;
            break;
        case TYPE_FUN_CALL:
            //printf("FUNCTION CALL\n");
            level--;
            break;
        case TYPE_RETURN:
            //printf("RETURN\n");
            level--;
            break;
        case TYPE_ASSIGNMENT:
            //printf("ASSIGNMENT\n");
            level--;
            break;
        case TYPE_IF_ELSE:
            //printf("IF ELSE\n");
            error = TraverseASTCodeGen(node->left->right);
            error = TraverseASTCodeGen(node->left->left);
            break;
        case TYPE_WHILE_CLOSED:
            //printf("WHILE\n");
            error = TraverseASTCodeGen(GetNode(node));
            break;
        case TYPE_IF_CLOSED:
            //printf("IF\n");
            error = TraverseASTCodeGen(GetCode(node));
            break;
        case TYPE_ELSE_CLOSED:
            //printf("ELSE\n");
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


