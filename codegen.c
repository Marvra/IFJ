/* TODO:    While - semi !
            Func. params - not done
            Func. return - not done
*/

#include "codegen.h"

#define FUNCTION_readstr \
"LABEL ifj_readstr\n" \
"CREATEFRAME\n" \
"PUSHFRAME\n" \
"DEFVAR LF@out\n" \
"READ LF@out string\n" \
"PUSHS LF@out\n" \
"POPFRAME\n" \
"RETURN\n\n"

#define FUNCTION_readi32 \
"LABEL ifj_readi32\n" \
"CREATEFRAME\n" \
"PUSHFRAME\n" \
"DEFVAR LF@out\n" \
"DEFVAR LF@err\n" \
"MOVE LF@err int@0\n" \
"READ LF@out int\n" \
"JUMPIFEQ $readi32_end LF@out nil@nil\n" \
"PUSHS LF@out\n" \
"POPFRAME\n" \
"RETURN\n" \
"LABEL $readi32_end\n" \
"MOVE LF@err int@1\n" \
"PUSHS nil@nil\n" \
"PUSHS LF@err\n" \
"POPFRAME\n" \
"RETURN\n\n"

#define FUNCTION_readf64 \
"LABEL ifj_readf64\n" \
"CREATEFRAME\n" \
"PUSHFRAME\n" \
"DEFVAR LF@out\n" \
"DEFVAR LF@err\n" \
"MOVE LF@err int@0\n" \
"READ LF@out float\n" \
"JUMPIFEQ $readf64_end LF@out nil@nil\n" \
"PUSHS LF@out\n" \
"POPFRAME\n" \
"RETURN\n" \
"LABEL $readf64_end\n" \
"MOVE LF@err int@1\n" \
"PUSHS nil@nil\n" \
"PUSHS LF@err\n" \
"POPFRAME\n" \
"RETURN\n\n"

#define FUNCTION_i2f \
"LABEL ifj_i2f\n" \
"PUSHFRAME\n" \
"DEFVAR LF@result\n" \
"INT2FLOAT LF@result LF@%1\n" \
"PUSHS LF@result\n" \
"POPFRAME\n" \
"RETURN\n\n"

#define FUNCTION_f2i \
"LABEL ifj_f2i\n" \
"PUSHFRAME\n" \
"DEFVAR LF@result\n" \
"FLOAT2INT LF@result LF@%1\n" \
"PUSHS LF@result\n" \
"POPFRAME\n" \
"RETURN\n\n"

#define FUNCTION_string \
"LABEL ifj_string\n" \
"PUSHFRAME\n" \
"DEFVAR LF@result\n" \
"MOVE LF@result LF@%1\n" \
"PUSHS LF@result\n" \
"POPFRAME\n" \
"RETURN\n\n"

#define FUNCTION_length \
"LABEL ifj_length\n" \
"PUSHFRAME\n" \
"DEFVAR LF@result\n" \
"STRLEN LF@result LF@%1\n" \
"PUSHS LF@result\n" \
"POPFRAME\n" \
"RETURN\n\n"

#define FUNCTION_concat \
"LABEL ifj_concat\n" \
"PUSHFRAME\n" \
"DEFVAR LF@result\n" \
"CONCAT LF@result LF@%1 LF@%2\n" \
"PUSHS LF@result\n" \
"POPFRAME\n" \
"RETURN\n\n"

#define FUNCTION_substring \
"LABEL ifj_substring\n" \
"PUSHFRAME\n" \
"DEFVAR LF@result\n" \
"DEFVAR LF@start\n" \
"DEFVAR LF@end\n" \
"DEFVAR LF@length\n" \
"DEFVAR LF@temp\n" \
"MOVE LF@start LF@%2\n" \
"MOVE LF@end LF@%3\n" \
"STRLEN LF@length LF@%1\n" \
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
"LABEL substring_end\n" \
"PUSHS LF@result\n" \
"POPFRAME\n" \
"RETURN\n" \
"LABEL substring_error\n" \
"PUSHS nil@nil\n" \
"POPFRAME\n" \
"RETURN\n\n"

#define FUNCTION_comp \
"LABEL ifj_strcmp\n" \
"PUSHFRAME \n" \
"DEFVAR LF@param1\n" \
"MOVE LF@param1 LF@%1\n" \
"DEFVAR LF@param2\n" \
"MOVE LF@param2 LF@%2\n" \
"DEFVAR LF@result\n" \
"DEFVAR LF@len1\n" \
"DEFVAR LF@len2\n" \
"STRLEN LF@len1 LF@param1\n" \
"STRLEN LF@len2 LF@param2\n" \
"LT LF@result LF@len1 LF@len2\n" \
"JUMPIFEQ $set_minus_one LF@result bool@true\n" \
"GT LF@result LF@len1 LF@len2\n" \
"JUMPIFEQ $set_one LF@result bool@true\n" \
"MOVE LF@result int@0\n" \
"JUMP end\n" \
"LABEL $set_minus_one\n" \
"MOVE LF@result int@-1\n" \
"JUMP end\n" \
"LABEL $set_one\n" \
"MOVE LF@result int@1\n" \
"LABEL end\n" \
"PUSHS LF@result\n" \
"POPFRAME\n" \
"RETURN\n\n"

#define FUNCTION_ord \
"LABEL ifj_ord\n" \
"PUSHFRAME\n" \
"DEFVAR LF@result\n" \
"DEFVAR LF@str\n" \
"DEFVAR LF@index\n" \
"DEFVAR LF@char\n" \
"DEFVAR LF@strlen\n" \
"DEFVAR LF@cond\n" \
"MOVE LF@str LF@%1\n" \
"MOVE LF@index LF@%2\n" \
"STRLEN LF@strlen LF@str\n" \
"JUMPIFEQ ord_out_of_bounds LF@strlen int@0\n" \
"LT LF@cond LF@index LF@strlen\n" \
"JUMPIFEQ ord_out_of_bounds LF@cond bool@false\n" \
"LT LF@cond LF@index int@0\n" \
"JUMPIFEQ ord_out_of_bounds LF@cond bool@true\n" \
"GETCHAR LF@char LF@str LF@index\n" \
"STRI2INT LF@result LF@str LF@index\n" \
"JUMP ord_end\n" \
"LABEL ord_out_of_bounds\n" \
"MOVE LF@result int@0\n" \
"LABEL ord_end\n" \
"PUSHS LF@result\n" \
"POPFRAME\n" \
"RETURN\n\n"

#define FUNCTION_chr \
"LABEL ifj_chr\n" \
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
    printf(FUNCTION_readstr);
    printf(FUNCTION_readi32);
    printf(FUNCTION_readf64);
    printf(FUNCTION_f2i);
    printf(FUNCTION_i2f);
    printf(FUNCTION_concat);
    printf(FUNCTION_string);
    printf(FUNCTION_length);
    printf(FUNCTION_substring);
    printf(FUNCTION_comp);
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
            printf("WRITE float@0x%fp+0\n",value);
        }else if(type == TYPE_STRING){
            char *string = WriteString(node->right);
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

char* concat(const char *s1, const char *s2) {
    // Check for NULL inputs
    if (s1 == NULL) s1 = "";
    if (s2 == NULL) s2 = "";

    // Allocate memory for the concatenated string
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for null-terminator
    if (result == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    // Concatenate strings
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char* removeFirstAndLast(const char* str) {
    // Check for NULL input or empty string
    if (str == NULL || str[0] == '\0') {
        return NULL;
    }

    // Check if string is too short (less than 2 characters)
    size_t len = strlen(str);
    if (len < 2) {
        return strdup(""); // Return empty string
    }

    // Allocate memory for the new string
    char* result = malloc(len - 1); // -1 to remove first and last chars
    if (result == NULL) {
        return NULL; // Memory allocation failed
    }

    // Copy the substring (skipping first and last characters)
    strncpy(result, str + 1, len - 2);
    result[len - 2] = '\0'; // Null-terminate the string

    return result;
}

typedef enum {
    STATE_NORMAL,
    STATE_BACKSLASH
} ParseState;

char* WriteString(ASTNode *node) {
    if (node == NULL) {
        return NULL;
    }

    char* string = node->data.str;
    string = removeFirstAndLast(string);
    if (string == NULL) {
        return NULL;
    }

    // Allocate a string large enough to handle the worst-case scenario
    char* results = malloc(strlen(string) * 5 + 1);
    if (results == NULL) {
        return NULL;
    }
    results[0] = '\0';  // Initialize as empty string

    char concat_string[10];  // Buffer for individual character conversion
    ParseState state = STATE_NORMAL;
    
    for (int i = 0; i < strlen(string); i++) {
        concat_string[0] = '\0';  // Reset concat_string

        switch (state) {
            case STATE_NORMAL:
                switch (string[i]) {
                    case 32:  // Space
                        strcpy(concat_string, "\\032");
                        break;
                    case 92:  // Backslash
                        state = STATE_BACKSLASH;
                        continue;
                    default:
                        concat_string[0] = string[i];
                        concat_string[1] = '\0';
                        break;
                }
                break;

            case STATE_BACKSLASH:
                switch (string[i]) {
                    case 'n':
                        strcpy(concat_string, "\\010");  // Newline escape
                        break;
                    case '"':
                        strcpy(concat_string, "\\034");  // Quote escape
                        break;
                    case '\\':
                        strcpy(concat_string, "\\092");  // Backslash escape
                        break;
                    case 'r':
                        strcpy(concat_string, "\\013");
                        break;
                    case 't':
                        strcpy(concat_string, "\\009");
                        break;
                    default:
                        // If not a special escape, output the original backslash and current char
                        strcpy(concat_string, "\\092");
                        // Prepare to handle the current character in the next iteration
                        i--;
                        break;
                }
                state = STATE_NORMAL;
                break;
        }

        // Temporary variable to store the new concatenated result
        char* temp = concat(results, concat_string);
        if (temp == NULL) {
            // Handle concatenation failure
            free(results);
            return NULL;
        }

        // Free the old results and update with the new concatenation
        free(results);
        results = temp;
    }

    return results;
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
    if(type == TYPE_VALUE_I32)
    {
        int value = node->data.i32;
        printf("PUSHS int@%d\n", value);
    }
    else if(type == TYPE_VALUE_F64)
    {
        float value = node->data.f64;
        printf("PUSHS float@0x%fp+0\n", value);
    }
    else if(type == TYPE_STRING)
    {
        char* string = WriteString(node);
        printf("PUSHS string@%s\n", string);
    }
    else if(type == TYPE_ID)
    {
        char *id = GetId(node);
        printf("PUSHS LF@%s\n", id);
    }
    else if(type == TYPE_ARGUMENT)
    {
        char *id = GetId(node);
        printf("PUSHS LF@%s\n", id);
    }
    else if(type == TYPE_FUN_CALL){
        char *functionId = GetId(node->left);
        // GOOD
        if(!strcmp(functionId, "ifj.readstr"))
        {
            printf("CALL ifj_readstr\n");
        }
        // GOOD
        else if(!strcmp(functionId, "ifj.readi32"))
        {
            printf("CALL ifj_readi32\n");
        }
        // NO IDEA
        else if(!strcmp(functionId, "ifj.readf64"))
        {
            printf("CALL ifj_readf64\n");
        }
        // GOOD
        else if(!strcmp(functionId, "ifj.i2f"))
        {
            printf("CREATEFRAME\n");
            CreateExpression(node->right);
            printf("DEFVAR TF@%1\n");
            printf("POPS TF@%1\n");
            printf("CALL ifj_i2f\n");
        }
        // GOOD
        else if(!strcmp(functionId, "ifj.f2i"))
        {
            printf("CREATEFRAME\n");
            CreateExpression(node->right);
            printf("DEFVAR TF@%1\n");
            printf("POPS TF@%1\n");
            printf("CALL ifj_f2i\n");
        }
        // GOOD
        else if(!strcmp(functionId, "ifj.string"))
        {
            printf("CREATEFRAME\n");
            CreateExpression(node->right);
            printf("DEFVAR TF@%1\n");
            printf("POPS TF@%1\n");
            printf("CALL ifj_string\n");
        }
        // GOOD
        else if(!strcmp(functionId, "ifj.length"))
        {
            printf("CREATEFRAME\n");
            CreateExpression(node->right);
            printf("DEFVAR TF@%1\n");
            printf("POPS TF@%1\n");
            printf("CALL ifj_length\n");
        }
        // GOOD
        else if(!strcmp(functionId, "ifj.concat"))
        {
            printf("CREATEFRAME\n");
            CreateExpression(node->right);
            CreateExpression(node->right->right);
            printf("DEFVAR TF@%1\n");
            printf("DEFVAR TF@%2\n");
            printf("POPS TF@%2\n");
            printf("POPS TF@%1\n");
            printf("CALL ifj_concat\n");
        }
        // GOOD
        else if(!strcmp(functionId, "ifj.substring"))
        {
            printf("CREATEFRAME\n");
            CreateExpression(node->right);
            CreateExpression(node->right->right);
            CreateExpression(node->right->right->right);
            printf("DEFVAR TF@%1\n");
            printf("DEFVAR TF@%2\n");
            printf("DEFVAR TF@%3\n");
            printf("POPS TF@%3\n");
            printf("POPS TF@%2\n");
            printf("POPS TF@%1\n");
            printf("CALL ifj_substring\n");
        }
        // GOOD
        else if(!strcmp(functionId, "ifj.strcmp"))
        {
            printf("CREATEFRAME\n");
            CreateExpression(node->right);
            CreateExpression(node->right->right);
            printf("DEFVAR TF@%1\n");
            printf("DEFVAR TF@%2\n");
            printf("POPS TF@%2\n");
            printf("POPS TF@%1\n");
            printf("CALL ifj_strcmp\n");
        }
        // GOOD
        else if(!strcmp(functionId, "ifj.ord"))
        {
            printf("CREATEFRAME\n");
            CreateExpression(node->right);
            CreateExpression(node->right->right);
            printf("DEFVAR TF@%1\n");
            printf("DEFVAR TF@%2\n");
            printf("POPS TF@%2\n");
            printf("POPS TF@%1\n");
            printf("CALL ifj_ord\n");
        }
        // GOOD
        else if(!strcmp(functionId, "ifj.chr"))
        {
            printf("CREATEFRAME\n");
            CreateExpression(node->right);
            printf("DEFVAR TF@%1\n");
            printf("POPS TF@%1\n");
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
            printf("IDIVS\n");
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
    else if(type == TYPE_ARGUMENT)
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
        else if (op == OP_GE)
        {
            printf("GTS\n");
            CreateExpression(node->left);
            CreateExpression(node->right);
            printf("EQS\n");
            printf("ORS\n");
        }
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

void StartIfElse(ASTNode *node, int cond)
{
    printf("CREATEFRAME\n");
    RelOperator(node->left->right);
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS $NOT$IF%d\n", cond);
    TraverseASTCodeGen(node->right->left);
    printf("JUMP $END$IFELSE%d\n", cond); 
    printf("LABEL $NOT$IF%d\n", cond);
    TraverseASTCodeGen(node->right->right); 
    printf("LABEL $END$IFELSE%d\n", cond);
}

void CreateWhile(ASTNode *node, int cond)
{
    printf("CREATEFRAME\n");
    printf("LABEL $WHILE$START%d\n", cond);
    RelOperator(node->left->right);
    printf("PUSHS bool@true\n");
    printf("JUMPIFEQS $WHILE$END%d\n", cond);
    TraverseASTCodeGen(node->right);
    printf("JUMP $WHILE$START%d\n", cond);
    printf("LABEL $WHILE$END%d\n", cond);
}

int TraverseASTCodeGen(ASTNode *node){
    int params = 1;
    int error = 0;
    static int whileCond = 1;
    static int ifElseCond = 1;
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
            ifElseCond++;
            StartIfElse(node, ifElseCond);
            error = TraverseASTCodeGen(node->left->right);
            error = TraverseASTCodeGen(node->left->left);
            level++;
            break;
        case TYPE_WHILE_CLOSED:
            whileCond++;
            CreateWhile(node, whileCond);
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