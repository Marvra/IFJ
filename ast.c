#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASTNode* CreateAST(){
    return CreateAstNode(TYPE_PROGRAM);
}

ASTNode* CreateAstNode(ASTNodeType type){
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if(node == NULL){
        printf(" CreateAstNode error");
        return NULL;
    }

    node->type = type;

    if(type == TYPE_VALUE){
        node->data.value = 0;
    }else if(type == TYPE_OPERATOR || type == TYPE_REL_OPERATOR){
        node->data.op = OP_DEFAULT;
    }else if(type == TYPE_DATA_TYPE || type == TYPE_RETURN_TYPE){
        node->data.type = T_DEFAULT;
    }else{
        node->data.str = NULL;
    }
    
    node->left = NULL;
    node->right = NULL;
    return node;
}

ASTNode* CreateCodeNode(ASTNode *node){
    if(node == NULL){
        printf(" CreateCodeNode error");
        return NULL;
    }

    ASTNode *temp = NULL;
    if(node->type == TYPE_FUN_DECL || node->type == TYPE_WHILE){
        if(node->right == NULL){
            node->right = CreateAstNode(TYPE_CODE);
            return node->right;
        }else{
            temp = node->right;
        }
    }else if(node->type == TYPE_IF_ELSE1){
        if(node->right != NULL){
            temp = node->right;
        }else if(node->left != NULL){
            temp = node->left;
        }else{
            printf(" CreateCodeNode error");
            return NULL;
        }
    }else{
        if(node->left == NULL){
            node->left = CreateAstNode(TYPE_CODE);
            return node->left;
        }else{
            temp = node->left;
        }
    }

    while(temp->left != NULL){
        temp = temp->left;
    }

    temp->left = CreateAstNode(TYPE_CODE);
    return temp->left;
}

ASTNode* CreateVarDeclNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("CreateVarDeclNode error");
        return NULL;
    }
    node->right = CreateAstNode(TYPE_VAR_DECL);
    return node->right;
}

ASTNode* CreateConDeclNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("CreateConDeclNode error");
        return NULL;
    }
    node->right = CreateAstNode(TYPE_CON_DECL);
    return node->right;
}

ASTNode* CreateFunDeclNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("CreateFunDeclNode error");
        return NULL;
    }
    node->right = CreateAstNode(TYPE_FUN_DECL);
    return node->right;
}

ASTNode* CreateIdNode(ASTNode *node, char *id){
    if(node == NULL){
        printf("CreateIdNode error");
        return NULL;
    }

    if(node->type == TYPE_VAR_DECL || node->type == TYPE_ASSIGNMENT || node->type == TYPE_FUN_DECL || node->type == TYPE_FUN_CALL){
        node->left = CreateAstNode(TYPE_ID);
        node->left->data.str = strdup(id);      
        if(node->left->data.str == NULL){
            fprintf(stderr, "AST NODE: Memory allocation failed\n");
            exit(-1);
        }
        return node;
    }else if(node->type == TYPE_WHILE || node->type == TYPE_IF_ELSE){
        if(node->left == NULL){
            printf("error");
            return NULL;
        }else{
            node->left->left = CreateAstNode(TYPE_ID);
            node->left->left->data.str = strdup(id);      
            if(node->left->left->data.str == NULL){
                fprintf(stderr, "AST NODE: Memory allocation failed\n");
                exit(-1);
            }
            return node;
        }
    }else{
        printf("CreateIdNode error");
        return NULL;
    }
}

ASTNode* CreateIfElseNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("CreateIfElseNode error");
        return NULL;
    }
    node->right = CreateAstNode(TYPE_IF_ELSE);
    node->right->right = CreateAstNode(TYPE_IF_ELSE1);
    node->right->left = CreateAstNode(TYPE_BLOCK);
    return node->right;
}

ASTNode* CreateIfNode(ASTNode *node){
    if(node->type != TYPE_IF_ELSE){
        printf("CreateIfNode error");
        return NULL;
    }
    node->right->left = CreateAstNode(TYPE_IF);
    return node->right;
}

ASTNode* CreateElseNode(ASTNode *node){
    if(node->type != TYPE_IF_ELSE1){
        printf("CreateElseNode error");
        return NULL;
    }
    node->right = CreateAstNode(TYPE_ELSE);
    return node;
}

ASTNode* CreateWhileNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("CreateWhileNode error");
        return NULL;
    }
    node->right = CreateAstNode(TYPE_WHILE);
    node->right->left = CreateAstNode(TYPE_WHILE1);
    return node->right;
}

ASTNode* CreateReturnNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("CreateReturnNode error");
        return NULL;
    }
    node->right = CreateAstNode(TYPE_RETURN);
    return node->right;
}

ASTNode* CreateFunCallNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("CreateFunCallNode error");
        return NULL;
    }
    node->right = CreateAstNode(TYPE_FUN_CALL);
    return node->right;
}

ASTNode* CreateArgumentNode(ASTNode *node, char *id){
    if(node->type != TYPE_FUN_CALL){
        printf("CreateArgumentNode error");
        return NULL;
    }

    ASTNode *temp = node;
    while(temp->right != NULL){
        temp = temp->right;
    }
    temp->right = CreateAstNode(TYPE_ARGUMENT);
    temp->right->data.str = strdup(id);
    if(temp->right->data.str == NULL){
        fprintf(stderr, "AST NODE: Memory allocation failed\n");
        exit(-1);
    }
    return node;
}

ASTNode* CreateParamNode(ASTNode *node, char *id){
    if(node->type != TYPE_FUN_DECL || node->left == NULL){
        printf("CreateParamNode error");
        return NULL;
    }

    ASTNode *temp = node;
    while(temp->left != NULL){
        temp = temp->left;
    }
    temp->left = CreateAstNode(TYPE_PARAMETER);
    temp->left->data.str = strdup(id);
    if(temp->left->data.str == NULL){
        fprintf(stderr, "AST NODE: Memory allocation failed\n");
        exit(-1);
    }
    return node;
    
}

ASTNode* CreateTypeNode(ASTNode *node, DataType type){
    if((node->type != TYPE_FUN_DECL && node->type != TYPE_VAR_DECL && node->type != TYPE_CON_DECL) || node->left == NULL ){
        printf("CreateTypeNode error");
        return NULL;
    }

    ASTNode *temp = node->left;
    while(temp->left != NULL){
        temp = temp->left;
    }

    if(temp->right != NULL){
        temp = node->left;
        if(node->type == TYPE_FUN_DECL){
            temp->right = CreateAstNode(TYPE_RETURN_TYPE);
            temp->right->data.type = type;
            return node;
        }
    }

    temp->right = CreateAstNode(TYPE_DATA_TYPE);
    temp->right->data.type = type;
    return node;
}

// int main(){
//     //test
//     ASTNode *ast = CreateAST();
//     ASTNode *temp = CreateCodeNode(ast);
//     ASTNode *temp1 = CreateCodeNode(ast);
//     ASTNode *temp2 = CreateCodeNode(ast);
//     ASTNode *temp3 = CreateCodeNode(temp1);
//     temp3 = CreateIfElseNode(temp1);
//     temp3 = CreateIdNode(temp3, "a");
//     temp3 = CreateFunCallNode(temp);
//     temp3 = CreateArgumentNode(temp3, "a");
//     temp3 = CreateArgumentNode(temp3, "b");
//     temp3 = CreateArgumentNode(temp3, "c");
//     temp3 = CreateFunDeclNode(temp2);
//     temp3 = CreateIdNode(temp3, "aaa");
//     temp3 = CreateParamNode(temp3, "ab");
//     temp3 = CreateParamNode(temp3, "abc");
// }

// ADDDED CHATGPT CREATED PRINT SHIT

// Helper function to print spaces for indentation
// Helper function to print spaces for indentation
void PrintIndent(int depth) {
    for (int i = 0; i < depth; i++) {
     printf(" ");
    }
}

// Helper function to display the AST node type as a string
const char* NodeTypeToString(ASTNodeType type) {
    switch (type) {
        case TYPE_PROGRAM: return "Program";
        case TYPE_CODE: return "Code";
        case TYPE_VAR_DECL: return "VarDecl";
        case TYPE_CON_DECL: return "ConDecl";
        case TYPE_FUN_DECL: return "FunDecl";
        case TYPE_ASSIGNMENT: return "Assignment";
        case TYPE_RETURN: return "Return";
        case TYPE_IF_ELSE: return "IfElse";
        case TYPE_IF_ELSE1: return "IfElse1";
        case TYPE_IF: return "If";
        case TYPE_ELSE: return "Else";
        case TYPE_BLOCK: return "Block";
        case TYPE_WHILE: return "While";
        case TYPE_WHILE1: return "While1";
        case TYPE_FUN_CALL: return "FunCall";
        case TYPE_ARGUMENT: return "Argument";
        case TYPE_PARAMETER: return "Parameter";
        case TYPE_ID: return "ID";
        case TYPE_OPERATOR: return "Operator";
        case TYPE_VALUE: return "Value";
        case TYPE_STRING: return "String";
        case TYPE_REL_OPERATOR: return "RelOperator";
        case TYPE_DATA_TYPE: return "DataType";
        case TYPE_RETURN_TYPE: return "ReturnType";
        default: return "Unknown";
    }
}

// Helper function to display operator type as a string
const char* OperatorToString(Operator op) {
    switch (op) {
        case OP_ADD: return "ADD (+)";
        case OP_SUB: return "SUB (-)";
        case OP_MUL: return "MUL (*)";
        case OP_DIV: return "DIV (/)";
        case OP_EQ: return "EQ (==)";
        case OP_NEQ: return "NEQ (!=)";
        case OP_GREATER: return "GREATER (>)";
        case OP_LESS: return "LESS (<)";
        case OP_GE: return "GE (>=)";
        case OP_LE: return "LE (<=)";
        case OP_DEFAULT: return "DEFAULT";
        default: return "Unknown";
    }
}

// Recursive function to display the AST, with internal depth and direction management
void DisplayASTHelper(ASTNode *node, int depth, const char* prefix) {
    if (node == NULL) return;

    // Print indentation for the current depth
    
    // Print the direction label (prefix), then node type
    printf("%s %s", prefix, NodeTypeToString(node->type));

    // Print additional node information if it exists
    if (node->type == TYPE_ID || node->type == TYPE_STRING || node->type == TYPE_PARAMETER || node->type == TYPE_ARGUMENT) {
        printf(": %s", node->data.str);
    } else if (node->type == TYPE_OPERATOR || node->type == TYPE_REL_OPERATOR) {
        printf(": %s", OperatorToString(node->data.op));
    } else if (node->type == TYPE_VALUE) {
        printf(": %f", node->data.value);
    } else if (node->type == TYPE_DATA_TYPE || node->type == TYPE_RETURN_TYPE) {
        printf(":");
        DisplayDataType(node->data.type);
    }
    // Recurse for left and right children, increasing depth and adding appropriate prefix
    if(node->left != NULL || node->right != NULL)
    {
        printf("\n");
        printf("DEPTH %i: ", depth);
        depth++;
        
        if (node->left != NULL) {
            DisplayASTHelper(node->left, depth, " L-->");
        }
        if (node->right != NULL) {
            DisplayASTHelper(node->right, depth, " R-->");
        }
        printf("\n");
    }
}


void DisplayDataType(DataType dataType) {
    switch (dataType) {
        case T_DEFAULT:
            printf("DEFAULT\n");
            break;
        case T_VOID:
            printf("VOID\n");
            break;
        case T_I32:
            printf("I32\n");
            break;
        case T_F64:
            printf("F64\n");
            break;
        case T_U8:
            printf("U8\n");
            break;
        default:
            printf("UNKNOWN\n");
            break;
    }
}

// Wrapper function that starts the AST display from depth 0
void DisplayAST(ASTNode *node) {
    DisplayASTHelper(node, 0, "Root-->");
}
// ADDDED CHATGPT CREATED PRINT SHIT