#ifndef AST_H
#define AST_H

typedef enum{
    TYPE_PROGRAM,
    TYPE_CODE,
    TYPE_VAR_DECL,
    TYPE_CON_DECL,
    TYPE_FUN_DECL,
    TYPE_ASSIGNMENT,
    TYPE_RETURN,
    TYPE_IF_ELSE,
    TYPE_IF_ELSE1,
    TYPE_IF,
    TYPE_IF_CLOSED,
    TYPE_ELSE,
    TYPE_ELSE_CLOSED,
    TYPE_BLOCK,
    TYPE_WHILE,
    TYPE_WHILE1,
    TYPE_WHILE_CLOSED,
    TYPE_FUN_CALL,
    TYPE_ARGUMENT,
    TYPE_PARAMETER,
    TYPE_ID,
    TYPE_OPERATOR,
    TYPE_VALUE_I32,
    TYPE_VALUE_F64,
    TYPE_STRING,
    TYPE_REL_OPERATOR,
    TYPE_DATA_TYPE,
    TYPE_RETURN_TYPE,
    TYPE_NULL
}ASTNodeType;

typedef enum{
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_EQ,         // ==
    OP_NEQ,        // !=
    OP_GREATER,    // >
    OP_LESS,       // <
    OP_GE,         // >=
    OP_LE,         // <=
    OP_DEFAULT     
}Operator;

typedef enum{
    T_DEFAULT,
    T_VOID,
    T_I32,
    T_F64,
    T_U8,
    T_I32_N,
    T_F64_N,
    T_U8_N
}DataType;

typedef union{
    Operator op;
    char *str;
    float f64;
    int i32;
    DataType type;
}ASTData;

typedef struct ASTNode{
    ASTNodeType type;
    ASTData data;
    struct ASTNode *left;
    struct ASTNode *right;
}ASTNode;

typedef struct StackNode{
    ASTNode *node;
    struct StackNode *next;
}StackNode;

typedef struct{
    StackNode *top;
}ASTStack;

ASTNode* CreateAST();
ASTNode* CreateAstNode(ASTNodeType type);
ASTNode* CreateCodeNode(ASTNode *node);
ASTNode* CreateVarDeclNode(ASTNode *node);
ASTNode* CreateConDeclNode(ASTNode *node);
ASTNode* CreateFunDeclNode(ASTNode *node);
ASTNode* CreateAssignemtNode(ASTNode *node);
ASTNode* CreateIdNode(ASTNode *node, char *id);
ASTNode* CreateIfElseNode(ASTNode *node);
ASTNode* CreateWhileNode(ASTNode *node);
ASTNode* CreateIfNode(ASTNode *node);
ASTNode* CreateElseNode(ASTNode *node);
ASTNode* CreateReturnNode(ASTNode *node);
ASTNode* CreateFunCallNode(ASTNode *node);
ASTNode* CreateFunCallExpressionsNode(ASTNode *node);
ASTNode* CreateArgumentNode(ASTNode *node, char *id);
ASTNode* CreateArgumentNodeI32(ASTNode *node, int value);
ASTNode* CreateArgumentNodeF64(ASTNode *node, float value);
ASTNode* CreateArgumentNodeU8(ASTNode *node, char *value);
ASTNode* CreateParamNode(ASTNode *node, char *id);
ASTNode* CreateTypeNode(ASTNode *node, DataType type);
ASTNode* CreateNullNode(ASTNode *node);

ASTNode* GetCode(ASTNode *node);
ASTNode* GetNode(ASTNode *node);
ASTNodeType GetNodeType(ASTNode *node);
DataType GetDataType(ASTNode *node);
DataType GetDataTypeCurrentNode(ASTNode *node);
ASTNode* GetIdNode(ASTNode *node);
ASTNode* GetParamNode(ASTNode *node);
ASTNode* GetArgNode(ASTNode *node);
char* GetId(ASTNode *node);
ASTNode* GetIfNode(ASTNode *node);
ASTNode* GetElseNode(ASTNode *node);
ASTNode* GetNoNullId(ASTNode *node);
ASTNode* GetConditionNode(ASTNode *node);
Operator GetOperator(ASTNode *node);
ASTNode* GetRightOperand(ASTNode *node);
ASTNode* GetLeftOperand(ASTNode *node);
int GetIntValue(ASTNode *node);
float GetFloatValue(ASTNode *node);

const char* OperatorToString(Operator op);
const char* NodeTypeToString(ASTNodeType type);
void DisplayDataType(DataType dataType);

ASTStack* CreateStackAST();
void PushAST(ASTStack *stack, ASTNode *node);
ASTNode* PopAST(ASTStack *stack);
void FreeStackAST(ASTStack *stack);
#endif