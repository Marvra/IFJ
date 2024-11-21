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
        exit(99);
    }

    node->type = type;

    if(type == TYPE_VALUE_I32){
        node->data.i32 = 0;
    }else if(type == TYPE_VALUE_F64){
        node->data.f64 = 0;
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
        exit(99);
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
            exit(99);
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
        exit(99);
    }
    node->right = CreateAstNode(TYPE_VAR_DECL);
    return node->right;
}

ASTNode* CreateConDeclNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_CON_DECL);
    return node->right;
}

ASTNode* CreateFunDeclNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_FUN_DECL);
    return node->right;
}

ASTNode* CreateAssignemtNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("CreateFunDeclNode error");
        return NULL;
    }
    node->right = CreateAstNode(TYPE_ASSIGNMENT);
    return node->right;
}

ASTNode* CreateIdNode(ASTNode *node, char *id){
    if(node == NULL){
        exit(99);
    }

    if(node->type == TYPE_VAR_DECL || node->type == TYPE_CON_DECL || node->type == TYPE_ASSIGNMENT || node->type == TYPE_FUN_DECL || node->type == TYPE_FUN_CALL){
        node->left = CreateAstNode(TYPE_ID);
        node->left->data.str = strdup(id);      
        if(node->left->data.str == NULL){
            fprintf(stderr, "AST NODE: Memory allocation failed\n");
            exit(99);
        }
        return node;
    }else if(node->type == TYPE_WHILE || node->type == TYPE_IF_ELSE){
        if(node->left == NULL){
            exit(99);
        }else{
            node->left->left = CreateAstNode(TYPE_ID);
            node->left->left->data.str = strdup(id);      
            if(node->left->left->data.str == NULL){
                fprintf(stderr, "AST NODE: Memory allocation failed\n");
                exit(99);
            }
            return node;
        }
    }else{
        exit(99);
    }
}

ASTNode* CreateIfElseNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_IF_ELSE);
    node->right->right = CreateAstNode(TYPE_BLOCK);
    node->right->left = CreateAstNode(TYPE_IF_ELSE1);
    return node->right;
}

ASTNode* CreateIfNode(ASTNode *node){
    if(node->type != TYPE_IF_ELSE){
        exit(99);
    }
    node->right->left = CreateAstNode(TYPE_IF);
    return node->right;
}

ASTNode* CreateElseNode(ASTNode *node){
    if(node->type != TYPE_IF_ELSE){
        exit(99);
    }
    node->right->right = CreateAstNode(TYPE_ELSE);
    return node;
}

ASTNode* CreateWhileNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_WHILE);
    node->right->left = CreateAstNode(TYPE_WHILE1);
    return node->right;
}

ASTNode* CreateReturnNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_RETURN);
    return node->right;
}

ASTNode* CreateFunCallNode(ASTNode *node){
    if((node->type != TYPE_CODE && node->type != TYPE_VAR_DECL && node->type != TYPE_CON_DECL && node->type != TYPE_ASSIGNMENT)|| node->right != NULL){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_FUN_CALL);
    return node->right;
}

ASTNode* CreateFunCallExpressionsNode(ASTNode *node){
    if(node==NULL){
        node = CreateAstNode(TYPE_FUN_CALL);
    }
    return node;
}

ASTNode* CreateArgumentNode(ASTNode *node, char *id){
    if(node->type != TYPE_FUN_CALL){
        exit(99);
    }

    ASTNode *temp = node;
    while(temp->right != NULL){
        temp = temp->right;
    }
    temp->right = CreateAstNode(TYPE_ARGUMENT);
    temp->right->data.str = strdup(id);
    if(temp->right->data.str == NULL){
        fprintf(stderr, "AST NODE: Memory allocation failed\n");
        exit(99);
    }
    return node;
}

ASTNode* CreateArgumentNodeI32(ASTNode *node, int value){
    if(node->type != TYPE_FUN_CALL){
        printf("CreateArgumentNode error");
        return NULL;
    }

    ASTNode *temp = node;
    while(temp->right != NULL){
        temp = temp->right;
    }
    temp->right = CreateAstNode(TYPE_VALUE_I32);
    temp->right->data.i32 = value;
    
    return node;
}

ASTNode* CreateArgumentNodeF64(ASTNode *node, float value){
    ASTNode *temp = node;
    while(temp->right != NULL){
        temp = temp->right;
    }
    temp->right = CreateAstNode(TYPE_VALUE_F64);
    temp->right->data.f64 = value;
    
    return node;
}

ASTNode* CreateArgumentNodeU8(ASTNode *node, char *string){
    ASTNode *temp = node;
    while(temp->right != NULL){
        temp = temp->right;
    }

    temp->right = CreateAstNode(TYPE_STRING);
    temp->right->data.str = strdup(string);
    if(temp->right->data.str == NULL){
        fprintf(stderr, "AST NODE: Memory allocation failed\n");
        exit(99);
    }
    
    return node;
}

ASTNode* CreateParamNode(ASTNode *node, char *id){
    ASTNode *temp = node;
    while(temp->left != NULL){
        temp = temp->left;
    }
    temp->left = CreateAstNode(TYPE_PARAMETER);
    temp->left->data.str = strdup(id);
    if(temp->left->data.str == NULL){
        fprintf(stderr, "AST NODE: Memory allocation failed\n");
        exit(99);
    }
    return node;
    
}

ASTNode* CreateTypeNode(ASTNode *node, DataType type){
    if((node->type != TYPE_FUN_DECL && node->type != TYPE_VAR_DECL && node->type != TYPE_CON_DECL) || node->left == NULL ){
        exit(99);
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

    if(node->left->right == NULL && node->type == TYPE_FUN_DECL && node->left->left == NULL){
        node->left->right = CreateAstNode(TYPE_RETURN_TYPE);
        node->left->right->data.type = type;
        return node;
    }
    
    temp->right = CreateAstNode(TYPE_DATA_TYPE);
    temp->right->data.type = type;
    return node;
}

ASTNode* CreateNullNode(ASTNode *node){
    if(node != NULL || (node->type != TYPE_VAR_DECL && node->type != TYPE_ASSIGNMENT && node->type != TYPE_CON_DECL)){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_NULL);
    return node;
}

// ------------------------- GET ---------------------------

ASTNode* GetCode(ASTNode *node){
    if(node == NULL){
        return NULL;
    }
    return node->left;
}

ASTNode* GetNode(ASTNode *node){
    if(node == NULL){
        return NULL;
    }
    return node->right;
}

ASTNodeType GetNodeType(ASTNode *node){
    return node->type;
}

DataType GetDataType(ASTNode *node){
    if(node->right == NULL){
        return T_DEFAULT;
    }
    return node->right->data.type;
}

DataType GetDataTypeCurrentNode(ASTNode *node){
    if(node == NULL){
        return T_DEFAULT;
    }
    return node->data.type;
}

ASTNode* GetIdNode(ASTNode *node){
    if(node == NULL || node->left == NULL){
        return NULL;
    }
    return node->left;
}

ASTNode* GetParamNode(ASTNode *node){
    if(node == NULL || node->left == NULL){
        return NULL;
    }
    return node->left;
}

ASTNode* GetArgNode(ASTNode *node){
    if(node == NULL || node->right == NULL){
        return NULL;
    }
    return node->right;
}

char* GetId(ASTNode *node){
    if(node == NULL){
        return NULL;
    }
    return node->data.str;
}

ASTNode* GetIfNode(ASTNode *node){
    if(node == NULL || node->right == NULL){
        return NULL;
    }
    return node->right->left;
}

ASTNode* GetElseNode(ASTNode *node){
    if(node == NULL || node->right == NULL){
        return NULL;
    }
    return node->right->right;
}

ASTNode* GetNoNullId(ASTNode *node){
    if(node == NULL || node->left == NULL){
        return NULL;
    }
    return node->left->left;
}

ASTNode* GetConditionNode(ASTNode *node){
    if(node == NULL || node->left == NULL){
        return NULL;
    }
    return node->left->right;
}

Operator GetOperator(ASTNode* node){
    if(node == NULL || (node->type != TYPE_OPERATOR && node->type != TYPE_REL_OPERATOR)){
        exit(99);
    }
    return node->data.op;
}

ASTNode* GetRightOperand(ASTNode *node){
    if(node == NULL){
        return NULL;
    }
    return node->right;
}

ASTNode* GetLeftOperand(ASTNode *node){
    if(node == NULL){
        return NULL;
    }
    return node->left;
}

int GetIntValue(ASTNode *node){
    if(node->type == TYPE_VALUE_I32){
        return node->data.i32;
    }
    return 0;
}

float GetFloatValue(ASTNode *node){
    if(node->type == TYPE_VALUE_F64){
        return node->data.f64;
    }
    return 0;
}

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
        case TYPE_IF_CLOSED: return "IfEnd";
        case TYPE_ELSE: return "Else";
        case TYPE_ELSE_CLOSED: return "ElseEnd";
        case TYPE_BLOCK: return "Block";
        case TYPE_WHILE: return "While";
        case TYPE_WHILE1: return "While1";
        case TYPE_WHILE_CLOSED: return "WhileEnd";
        case TYPE_FUN_CALL: return "FunCall";
        case TYPE_ARGUMENT: return "Argument";
        case TYPE_PARAMETER: return "Parameter";
        case TYPE_ID: return "ID";
        case TYPE_OPERATOR: return "Operator";
        case TYPE_VALUE_I32: return "ValueI32";
        //case TYPE_VALUE: return "Value"; ---------------------------------------------------------------------------------------------------
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
    } /*else if (node->type == TYPE_VALUE) {
        printf(": %f", node->data.value);    -------------------------------------------------------------------------------------------------------------
    }*/ else if (node->type == TYPE_DATA_TYPE || node->type == TYPE_RETURN_TYPE) {
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

ASTStack* CreateStackAST() {
    ASTStack *stack = (ASTStack*)malloc(sizeof(ASTStack));
    if (!stack) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(99);
    }
    stack->top = NULL;
    return stack;
}

void PushAST(ASTStack *stack, ASTNode *node) {
    StackNode *newNode = (StackNode*)malloc(sizeof(StackNode));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(99);
    }
    newNode->node = node;
    newNode->next = stack->top;
    stack->top = newNode;
}

ASTNode* PopAST(ASTStack *stack) {
    if (stack->top == NULL) {
        return NULL;
    }
    StackNode *topNode = stack->top;
    ASTNode *node = topNode->node;
    stack->top = topNode->next;
    free(topNode);
    return node;
}


void FreeStackAST(ASTStack *stack) {
    StackNode *current = stack->top;
    while (current != NULL) {
        StackNode *next = current->next;
        free(current);
        current = next;
    }
    free(stack);
}