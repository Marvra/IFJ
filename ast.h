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
    TYPE_ELSE,
    TYPE_BLOCK,
    TYPE_WHILE,
    TYPE_WHILE1,
    TYPE_WHILE_CLOSED,
    TYPE_FUN_CALL,
    TYPE_ARGUMENT,
    TYPE_PARAMETER,
    TYPE_ID,
    TYPE_OPERATOR,
    TYPE_VALUE,
    TYPE_STRING,
    TYPE_REL_OPERATOR,
    TYPE_DATA_TYPE,
    TYPE_RETURN_TYPE
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
    OP_LE,
    OP_DEFAULT         // <=
}Operator;

typedef enum{
    T_DEFAULT,
    T_VOID,
    T_I32,
    T_F64,
    T_U8
}DataType;

typedef union{
    Operator op;
    char *str;
    float value;
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
ASTNode* CreateIdNode(ASTNode *node, char *id);
ASTNode* CreateIfElseNode(ASTNode *node);
ASTNode* CreateWhileNode(ASTNode *node);
ASTNode* CreateIfNode(ASTNode *node);
ASTNode* CreateElseNode(ASTNode *node);
ASTNode* CreateReturnNode(ASTNode *node);
ASTNode* CreateFunCallNode(ASTNode *node);
ASTNode* CreateArgumentNode(ASTNode *node, char *id);
ASTNode* CreateParamNode(ASTNode *node, char *id);
ASTNode* CreateTypeNode(ASTNode *node, DataType type);

ASTNode* GetCode(ASTNode *node);
ASTNode* GetNode(ASTNode *node);
ASTNodeType GetNodeType(ASTNode *node);
DataType GetDataType(ASTNode *node);
ASTNode* GetIdNode(ASTNode *node);
ASTNode* GetParamNode(ASTNode *node);
char* GetId(ASTNode *node);
ASTNode* GetIfNode(ASTNode *node);
ASTNode* GetElseNode(ASTNode *node);
ASTNode* GetNoNullId(ASTNode *node);
ASTNode* GetConditionNode(ASTNode *node);

/// added chagpt print shits
void DisplayAST(ASTNode *node);
void DisplayASTHelper(ASTNode *node, int depth, const char* prefix);
const char* OperatorToString(Operator op);
const char* NodeTypeToString(ASTNodeType type);
void PrintIndent(int depth);
void DisplayDataType(DataType dataType);

//chatgpt
ASTStack* CreateStackAST();
void PushAST(ASTStack *stack, ASTNode *node);
ASTNode* PopAST(ASTStack *stack);
void FreeStackAST(ASTStack *stack);
#endif