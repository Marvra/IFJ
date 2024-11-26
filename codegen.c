#include "codegen.h"

// Funkce
void appendFunctionName(char* function_id) {
    printf("LABEL $%s\n", function_id);
}

void appendFunctionParam(char* param_id)
{ 
    printf("DEFVAR LF@%s\n",param_id);	
}

// DataTypes
void dataType(char* id,DataType type, char* input)
{
    printf("WRITE GF@%s\n", input);

    switch (type)
    {
        case T_F64:
        case T_F64_N:
            printf("READ LF@$%s int\n", id);
            break;
        case T_I32:
        case T_I32_N:
            printf("READ LF@$%s float\n", id);
            break;
        case T_U8:
        case T_U8_N:
            printf("READ LF@$%s string\n", id);
            break;
    }
}

// Var decleration

void varDeclaration(char *var_id, DataType type, char* input) 
{
    printf("DEFVAR LF@$%s\n", var_id);
}

void assignment(char *assig_id)
{
    printf("LABEL $%s\n", assig_id);
}

// header, jump on main
void header()
{
    printf(".IFJcode24\n");

    printf("JUMP $$main");
}

void expression(char *id, Operator operator)
{
    switch(operator)
    {
        case OP_ADD:
            printf("ADD LF@$%s TF@%value1 TF@%value2", id);
            break;
        case OP_SUB:
            printf("SUB LF@$%s TF@%value1 TF@%value2", id);
            break;
        case OP_MUL:
            printf("MUL LF@$%s TF@%value1 TF@%value2", id);
            break;
        case OP_DIV:
            printf("DIV LF@$%s TF@%value1 TF@%value2", id);
            break;
    }
}

void relationalContidion(char *id, Operator operator)
{
    switch(operator)
    {
        case OP_EQ:
            printf("EQ LF@$%s TF@%value1 TF@%value2", id);
            break;
        case OP_GREATER:
            printf("GT LF@$%s TF@%value1 TF@%value2", id);
            break;
        case OP_LESS:
            printf("LT LF@$%s TF@%value1 TF@%value2", id);
            break;
        case OP_GE:

            break;
        case OP_NEQ:
            break;
        case OP_LE:
            break;
    }
}


