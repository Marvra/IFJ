#include "codegen.h"

// Print astNOde
void print_ast_node(ASTNode* node, int depth) {
    if (!node) return;

    // Print indentation
    for (int i = 0; i < depth; i++) printf("  ");

    // Print node type and specific details
    printf("%s", NodeTypeToString(node->type));
    switch (node->type) {
        case TYPE_ID: printf(" [%s]", node->data.str); break;
        case TYPE_VALUE_I32: printf(" [%d]", node->data.i32); break;
        case TYPE_VALUE_F64: printf(" [%f]", node->data.f64); break;
        case TYPE_OPERATOR: printf(" [%s]", OperatorToString(node->data.op)); break;
        case TYPE_DATA_TYPE: 
            printf(" [");
            DisplayDataType(node->data.type);
            printf("]");
            break;
        default: break;
    }
    printf("\n");

    // Recursively print children
    if (node->left) print_ast_node(node->left, depth + 1);
    if (node->right) print_ast_node(node->right, depth + 1);
}

void print_ast(ASTNode* node) {
    print_ast_node(node, 0);
}

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
            if (node->right->type == TYPE_OPERATOR)
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
            }
            level--;
            break;
        case TYPE_CON_DECL:
            CreateVariable(GetId(node->left));
            ExpressionsOperand(GetId(node->left), node->right);
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
            if (node->right->type == TYPE_OPERATOR)
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
            }
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

