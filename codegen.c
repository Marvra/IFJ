#include "codegen.h"

#define FUNCTION_substring \
"LABEL ifj_substring\n" \
"CREATEFRAME\n" \
"PUSHFRAME\n" \
\
"DEFVAR LF@result\n" \
"DEFVAR LF@start\n" \
"DEFVAR LF@end\n" \
"DEFVAR LF@length\n" \
"DEFVAR LF@temp\n" \
\
"MOVE LF@start LF@%2\n" \
"MOVE LF@end LF@%3\n" \
"STRLEN LF@length LF@%1\n" \
\
"# Kontrola platnosti indexů\n" \
"LT LF@temp LF@start int@0\n" \
"JUMPIFEQ substring_error LF@temp bool@true\n" \
"LT LF@temp LF@end int@0\n" \
"JUMPIFEQ substring_error LF@temp bool@true\n" \
"GT LF@temp LF@start LF@length\n" \
"JUMPIFEQ substring_error LF@temp bool@true\n" \
"GT LF@temp LF@end LF@length\n" \
"JUMPIFEQ substring_error LF@temp bool@true\n" \
"GT LF@temp LF@start LF@end\n" \
"JUMPIFEQ substring_error LF@temp bool@true\n" \
\
"# Vytvoření podřetězce\n" \
"DEFVAR LF@i\n" \
"MOVE LF@i LF@start\n" \
"MOVE LF@result string@\n" \
"LABEL substring_loop\n" \
"LT LF@temp LF@i LF@end\n" \
"JUMPIFEQ substring_end LF@temp bool@false\n" \
"GETCHAR LF@temp LF@%1 LF@i\n" \
"CONCAT LF@result LF@result LF@temp\n" \
"ADD LF@i LF@i int@1\n" \
"JUMP substring_loop\n" \
\
"LABEL substring_end\n" \
"PUSHS LF@result\n" \
"POPFRAME\n" \
"RETURN\n" \
\
"LABEL substring_error\n" \
"PUSHS nil@nil\n" \
"POPFRAME\n" \
"RETURN\n\n"

#define FUNCTION_ord \
"LABEL ifj_ord\n" \
"CREATEFRAME\n" \
"PUSHFRAME\n" \
\
"DEFVAR LF@result\n" \
"DEFVAR LF@length\n" \
"DEFVAR LF@index\n" \
"DEFVAR LF@char\n" \
\
"MOVE LF@index LF@%2\n" \
"STRLEN LF@length LF@%1\n" \
\
"# Kontrola platnosti indexu\n" \
"LT LF@result LF@index int@0\n" \
"JUMPIFEQ ord_error LF@result bool@true\n" \
"GT LF@result LF@index LF@length\n" \
"JUMPIFEQ ord_error LF@result bool@true\n" \
\
"# Získání ASCII hodnoty znaku\n" \
"STRI2INT LF@result LF@%1 LF@index\n" \
"PUSHS LF@result\n" \
"POPFRAME\n" \
"RETURN\n" \
\
"LABEL ord_error\n" \
"MOVE LF@result int@0\n" \
"PUSHS LF@result\n" \
"POPFRAME\n" \
"RETURN\n\n"

#define FUNCTION_chr \
"LABEL ifj_chr\n" \
"CREATEFRAME\n" \
"PUSHFRAME\n" \
\
"DEFVAR LF@result\n" \
"INT2CHAR LF@result LF@%1\n" \
\
"PUSHS LF@result\n" \
"POPFRAME\n" \
"RETURN\n\n"

// Zacatek printu
void PrintASTNodeType(ASTNode *node) {
    if (node == NULL) {
        printf("Node is NULL\n");
        return;
    }
    
    printf("AST Node Type: %s\n", NodeTypeToString(node->type));
}
// Konec printu

void CreateHeader()
{
    printf(".IFJcode24\n");
    printf("JUMP $main\n\n");
    printf(FUNCTION_substring);
    printf(FUNCTION_ord);
    printf(FUNCTION_chr);
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

void CreateFunctionCall(ASTNode *node) {
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
    }
    else
    {
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
        char *functionId = GetId(node->left);
        // GOOD
        if(!strcmp(functionId, "ifj.readstr"))
        {
            printf("CREATEFRAME\n");
            printf("DEFVAR TF@res\n");
            printf("READ TF@res string\n");
            printf("PUSHS TF@res\n");
        }
        // GOOD
        else if(!strcmp(functionId, "ifj.readi32"))
        {
            printf("CREATEFRAME\n");
            printf("DEFVAR TF@res\n");
            printf("READ TF@res int\n");
            printf("PUSHS TF@res\n");

        }
        // NOT TESTED
        else if(!strcmp(functionId, "ifj.readf64"))
        {
            printf("CREATEFRAME\n");
            printf("DEFVAR TF@res\n");
            printf("READ TF@res float\n");
            printf("PUSHS TF@res\n");
        }
        // NOT TESTED
        else if(!strcmp(functionId, "ifj.i2f"))
        {
            int value = GetIntValue(node->right);
            printf("CREATEFRAME\n");
            printf("DEFVAR TF@res\n");
            printf("INT2FLOAT TF@res int@%d\n", value);
            printf("PUSHS TF@res\n");
        }
        // NOT TESTED
        else if(!strcmp(functionId, "ifj.f2i"))
        {
            float value = GetFloatValue(node->right);
            printf("CREATEFRAME\n");
            printf("DEFVAR TF@res\n");
            printf("FLOAT@INT TF@res float@%f\n", value);
            printf("PUSHS TF@res\n");
        }
        // GOOD
        else if(!strcmp(functionId, "ifj.string"))
        {
            char* value = GetId(node->right);
            printf("CREATEFRAME\n");
            printf("DEFVAR TF@res\n");
            printf("MOVE TF@res string@%s\n", value);
            printf("PUSHS TF@res\n");
        }
        // GOOD
        else if(!strcmp(functionId, "ifj.length"))
        {
            char* value = GetId(node->right->right);
            printf("CREATEFRAME\n");
            printf("DEFVAR TF@res\n");
            printf("STRLEN TF@res string@%s\n", value);
            printf("PUSHS TF@res\n");
        }
        // NOT TESTED
        else if(!strcmp(functionId, "ifj.concat"))
        {
            char* value1 = GetId(node->right);
            char* value2 = GetId(node->right->right);
            printf("CREATEFRAME\n");
            printf("DEFVAR TF@res\n");
            printf("CONCAT TF@res string@%s string@%s\n", value1, value2);
            printf("PUSHS TF@res\n");
        }
        // Called with builtin functions
        // NOT TESTED
        else if(!strcmp(functionId, "ifj.substring"))
        {
            int value1 = GetIntValue(node->right);
            int value2 = GetIntValue(node->right->right);
            int value3 = GetIntValue(node->right->right->right);
            printf("CREATEFRAME\n");
            printf("DEFVAR TF@%1\n");
            printf("DEFVAR TF@%2\n");
            printf("DEFVAR TF@%3\n");
            printf("MOVE TF@%1 string@%d\n", value1);
            printf("MOVE TF@%2 int@%d\n", value2);
            printf("MOVE TF@%3 int@%d\n", value3);
            printf("CALL ifj_substring\n");
        }
        // NOT TESTED
        else if(!strcmp(functionId, "ifj.ord"))
        {
            int value1 = GetIntValue(node->right);
            int value2 = GetIntValue(node->right->right);
            printf("CREATEFRAME\n");
            printf("DEFVAR TF@%1\n");
            printf("DEFVAR TF@%2\n");
            printf("MOVE TF@%1 int@%d\n", value1);
            printf("MOVE TF@%1 int@%d\n", value2);
            printf("CALL ifj_ord\n");
        }
        // NOT TESTED
        else if(!strcmp(functionId, "ifj.chr"))
        {
            int value = GetIntValue(node->right);
            printf("CREATEFRAME\n");
            printf("DEFVAR TF@%1\n");
            printf("MOVE TF@%1 int@%d\n", value);
            printf("CALL ifj_chr\n");
        }
        else
        {
            printf("CALL $%s\n", functionId);
        }
    }
    else if(type == TYPE_OPERATOR)
    {
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

void RelOperator(ASTNode *node)
{
    ASTNodeType type = GetNodeType(node);
    if(type == TYPE_VALUE_I32)
    {
        int value = node->data.i32;
        printf("PUSHS int@%d\n", value);
    }
    else if(type == TYPE_VALUE_F64)
    {
        float value = node->data.f64;
        printf("PUSHS float@%f\n", value);
    }
    else if(type == TYPE_ID)
    {
        char *id = GetId(node);
        printf("PUSHS LF@%s\n", id);
    }
    else if (type==TYPE_REL_OPERATOR)
    {
        CreateExpression(node->left);
        CreateExpression(node->right);
        Operator op = GetOperator(node);
        if (op == OP_EQ)
        {
            printf("EQS\n");
        }
        else if (op == OP_GREATER)
        {
            printf("GTS\n");
        }
        else if(op == OP_LESS)
        {
            printf("LTS\n");
        }
        else if (op == OP_NEQ)
        {
            printf("EQS\n");
            printf("NOTS\n");
        }
        // I don't know if this work
        else if (op == OP_GE)
        {
            printf("GTS\n");
            CreateExpression(node->left);
            CreateExpression(node->right);
            printf("EQS\n");
            printf("ORS\n");
        }
        // I don't know if this work
        else if (op == OP_LE)
        {
            printf("LTS\n");
            CreateExpression(node->left);
            CreateExpression(node->right);
            printf("EQS\n");
            printf("ORS\n");
        }
    }
}

void CreateIfElse(ASTNode *node, int cond)
{
    printf("CREATEFRAME\n");
    printf("LABEL $START$IFELSE%d\n", cond);
    RelOperator(node->left->right);
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS $NOT$IF%d\n", cond);
    TraverseASTCodeGen(node->right->left);
    printf("JUMP $END$IFELSE%d\n", cond); 
    printf("LABEL $NOT$IF%d\n", cond);
    TraverseASTCodeGen(node->right->right); 
    printf("LABEL $END$IFELSE%d\n", cond);
}

int TraverseASTCodeGen(ASTNode *node){
    int params = 1;
    int error = 0;
    int cond = 1;
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
            CreateExpression(node->right);
            printf("POPS LF@%s\n",GetId(node->left));
            level--;
            break;
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
            level--;
            break;
        case TYPE_IF_ELSE:
            CreateIfElse(node, cond);
            error = TraverseASTCodeGen(node->left->right);
            error = TraverseASTCodeGen(node->left->left);
            level++;
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