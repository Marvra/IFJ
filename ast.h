#ifndef AST_H
#define AST_H

typedef enum{
    TYPE_PROGRAM,
    TYPE_CODE,
    TYPE_VAR_DECL,
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
    TYPE_FUN_CALL,
    TYPE_ARGUMENT,
    TYPE_PARAMETER,
    TYPE_ID,
    TYPE_OPERATOR,
    TYPE_VALUE,
    TYPE_STRING,
    TYPE_REL_OPERATOR,
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

typedef union{
    Operator op;
    char *str;
    float value;
}ASTData;

typedef struct ASTNode{
    ASTNodeType type;
    ASTData data;
    struct ASTNode *left;
    struct ASTNode *right;
}ASTNode;

ASTNode* CreateAST();
ASTNode* CreateNode(ASTNodeType type);
ASTNode* CreateCodeNode(ASTNode *node);
ASTNode* CreateVarDeclNode(ASTNode *node);
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
#endif