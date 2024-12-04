/**
 * @file ast.c
 * @author Robin Kurilla
 * @brief  source file for abstract syntax tree
 */

#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief creates AST
 * @return created AST structure
 */
ASTNode* CreateAST(){
    return CreateAstNode(TYPE_PROGRAM);
}

/**
 * @brief creates AST node
 * @param type node type
 * @return created node
 */
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

/**
 * @brief creates TYPE_CODE node
 * @param node higher node
 * @return created code node
 */
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

/**
 * @brief creates TYPE_VAR_DECL node - variable
 * @param node higher node
 * @return created var declaration node
 */
ASTNode* CreateVarDeclNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_VAR_DECL);
    return node->right;
}

/**
 * @brief creates TYPE_CON_DECL node - constant
 * @param node higher node
 * @return created constant declaration node
 */
ASTNode* CreateConDeclNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_CON_DECL);
    return node->right;
}

/**
 * @brief creates TYPE_FUN_DECL node - function declaration
 * @param node higher node
 * @return created function declaration node
 */
ASTNode* CreateFunDeclNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_FUN_DECL);
    return node->right;
}

/**
 * @brief creates TYPE_ASSIGNMENT node
 * @param node higher node
 * @return created assignment node
 */
ASTNode* CreateAssignemtNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("CreateFunDeclNode error");
        return NULL;
    }
    node->right = CreateAstNode(TYPE_ASSIGNMENT);
    return node->right;
}

/**
 * @brief creates TYPE_ID node - identificator
 * @param node higher node
 * @return higher node
 */
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

/**
 * @brief creates TYPE_IF_ELSE node
 * @param node higher node
 * @return created if else node
 */
ASTNode* CreateIfElseNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_IF_ELSE);
    node->right->right = CreateAstNode(TYPE_BLOCK);
    node->right->left = CreateAstNode(TYPE_IF_ELSE1);
    return node->right;
}

/**
 * @brief creates TYPE_IF node
 * @param node higher node
 * @return helper node 
 */
ASTNode* CreateIfNode(ASTNode *node){
    if(node->type != TYPE_IF_ELSE){
        exit(99);
    }
    node->right->left = CreateAstNode(TYPE_IF);
    return node->right;
}

/**
 * @brief creates TYPE_ELSE node
 * @param node higher node
 * @return higher node
 */
ASTNode* CreateElseNode(ASTNode *node){
    if(node->type != TYPE_IF_ELSE){
        exit(99);
    }
    node->right->right = CreateAstNode(TYPE_ELSE);
    return node;
}

/**
 * @brief creates TYPE_WHILE node
 * @param node higher node
 * @return created while node
 */
ASTNode* CreateWhileNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_WHILE);
    node->right->left = CreateAstNode(TYPE_WHILE1);
    return node->right;
}

/**
 * @brief creates TYPE_RETURN node
 * @param node higher node
 * @return created return node
 */
ASTNode* CreateReturnNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_RETURN);
    return node->right;
}

/**
 * @brief creates TYPE_FUN_CALL node - function call
 * @param node higher node
 * @return created function call node
 */
ASTNode* CreateFunCallNode(ASTNode *node){
    if((node->type != TYPE_CODE && node->type != TYPE_VAR_DECL && node->type != TYPE_CON_DECL && node->type != TYPE_ASSIGNMENT)|| node->right != NULL){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_FUN_CALL);
    return node->right;
}

/**
 * @brief creates TYPE_FUN_CALL node when expression missing
 * @param node expression node
 * @return created node or expression node
 */
ASTNode* CreateFunCallExpressionsNode(ASTNode *node){
    if(node==NULL){
        node = CreateAstNode(TYPE_FUN_CALL);
    }
    return node;
}

/**
 * @brief creates TYPE_ARGUMENT node
 * @param node higher node
 * @param id argument identificator
 * @return higher node
 */
ASTNode* CreateArgumentNode(ASTNode *node, char *id){
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

/**
 * @brief creates TYPE_NULL node - null as argument
 * @param node higher node
 * @return higher node
 */
ASTNode* CreateArgumentNullNode(ASTNode *node){
    ASTNode *temp = node;
    while(temp->right != NULL){
        temp = temp->right;
    }
    temp->right = CreateAstNode(TYPE_NULL);
    return node;
}

/**
 * @brief creates TYPE_VALUE_I32 node - i32 as argument
 * @param node higher node
 * @param value value
 * @return higher node
 */
ASTNode* CreateArgumentNodeI32(ASTNode *node, int value){
    ASTNode *temp = node;
    while(temp->right != NULL){
        temp = temp->right;
    }
    temp->right = CreateAstNode(TYPE_VALUE_I32);
    temp->right->data.i32 = value;
    
    return node;
}

/**
 * @brief creates TYPE_VALUE_F64 node - f64 as argument
 * @param node higher node
 * @param value value
 * @return higher node
 */
ASTNode* CreateArgumentNodeF64(ASTNode *node, float value){
    ASTNode *temp = node;
    while(temp->right != NULL){
        temp = temp->right;
    }
    temp->right = CreateAstNode(TYPE_VALUE_F64);
    temp->right->data.f64 = value;
    
    return node;
}

/**
 * @brief creates TYPE_VALUE_U8 node - string as argument
 * @param node higher node
 * @param value value
 * @return higher node
 */
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

/**
 * @brief creates TYPE_PARAMETER node
 * @param node higher node
 * @param id parameter identificator
 * @return higher node
 */
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

/**
 * @brief creates TYPE_DATA_TYPE or TYPE_RETURN_TYPE node - identificator data type or function return type
 * @param node higher node
 * @param type data/return type
 * @return higher node
 */
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

/**
 * @brief creates TYPE_NULL node
 * @param node higher node
 * @return higher node
 */
ASTNode* CreateNullNode(ASTNode *node){
    if(node != NULL || (node->type != TYPE_VAR_DECL && node->type != TYPE_ASSIGNMENT && node->type != TYPE_CON_DECL)){
        exit(99);
    }
    node->right = CreateAstNode(TYPE_NULL);
    return node;
}

// ------------------------- GET ---------------------------

/**
 * @brief gets code node
 * @param node higher node
 * @return code node from node
 */
ASTNode* GetCode(ASTNode *node){
    if(node == NULL){
        return NULL;
    }
    return node->left;
}

/**
 * @brief gets right node
 * @param node higher node
 * @return right node from node
 */
ASTNode* GetNode(ASTNode *node){
    if(node == NULL){
        return NULL;
    }
    return node->right;
}

/**
 * @brief gets node type
 * @param node node from which we want type
 * @return node type
 */
ASTNodeType GetNodeType(ASTNode *node){
    return node->type;
}

/**
 * @brief gets data type
 * @param node higher node - id/param
 * @return node data type or T_DEFAULT
 */
DataType GetDataType(ASTNode *node){
    if(node->right == NULL){
        return T_DEFAULT;
    }
    return node->right->data.type;
}

/**
 * @brief gets data type
 * @param node current node
 * @return node data type from current node or T_DEFAULT
 */
DataType GetDataTypeCurrentNode(ASTNode *node){
    if(node == NULL){
        return T_DEFAULT;
    }
    return node->data.type;
}

/**
 * @brief gets id node
 * @param node higher node
 * @return id node or null
 */
ASTNode* GetIdNode(ASTNode *node){
    if(node == NULL || node->left == NULL){
        return NULL;
    }
    return node->left;
}

/**
 * @brief gets param node
 * @param node higher node
 * @return param node or null
 */
ASTNode* GetParamNode(ASTNode *node){
    if(node == NULL || node->left == NULL){
        return NULL;
    }
    return node->left;
}

/**
 * @brief gets argument node
 * @param node higher node
 * @return argument node or null
 */
ASTNode* GetArgNode(ASTNode *node){
    if(node == NULL || node->right == NULL){
        return NULL;
    }
    return node->right;
}

/**
 * @brief gets id
 * @param node node from which we want id
 * @return id or null
 */
char* GetId(ASTNode *node){
    if(node == NULL){
        return NULL;
    }
    return node->data.str;
}

/**
 * @brief gets if node
 * @param node higher node
 * @return if node or null
 */
ASTNode* GetIfNode(ASTNode *node){
    if(node == NULL || node->right == NULL){
        return NULL;
    }
    return node->right->left;
}

/**
 * @brief gets else node
 * @param node higher node
 * @return else node or null
 */
ASTNode* GetElseNode(ASTNode *node){
    if(node == NULL || node->right == NULL){
        return NULL;
    }
    return node->right->right;
}

/**
 * @brief gets no null id node
 * @param node higher node
 * @return id node or null
 */
ASTNode* GetNoNullId(ASTNode *node){
    if(node == NULL || node->left == NULL){
        return NULL;
    }
    return node->left->left;
}

/**
 * @brief gets condition node
 * @param node higher node
 * @return condition node or null
 */
ASTNode* GetConditionNode(ASTNode *node){
    if(node == NULL || node->left == NULL){
        return NULL;
    }
    return node->left->right;
}

/**
 * @brief gets operator
 * @param node operator node
 * @return operator
 */
Operator GetOperator(ASTNode* node){
    if(node == NULL || (node->type != TYPE_OPERATOR && node->type != TYPE_REL_OPERATOR)){
        exit(99);
    }
    return node->data.op;
}

/**
 * @brief gets right node
 * @param node higher node
 * @return right node or null
 */
ASTNode* GetRightOperand(ASTNode *node){
    if(node == NULL){
        return NULL;
    }
    return node->right;
}

/**
 * @brief gets left node
 * @param node higher node
 * @return left node or null
 */
ASTNode* GetLeftOperand(ASTNode *node){
    if(node == NULL){
        return NULL;
    }
    return node->left;
}

/**
 * @brief gets i32 value
 * @param node i32 value node
 * @return int value
 */
int GetIntValue(ASTNode *node){
    if(node->type == TYPE_VALUE_I32){
        return node->data.i32;
    }
    return 0;
}

/**
 * @brief gets f64 value
 * @param node f64 value node
 * @return float value
 */
float GetFloatValue(ASTNode *node){
    if(node->type == TYPE_VALUE_F64){
        return node->data.f64;
    }
    return 0;
}

/**
 * @brief frees ast
 * @param node AST root
 */
void FreeAST(ASTNode *node){
    if(node == NULL){
        return;
    }
    FreeAST(node->right);
    FreeAST(node->left);
    if(node->type == TYPE_ID || node->type == TYPE_ARGUMENT || node->type == TYPE_PARAMETER || node->type == TYPE_STRING){
        free(node->data.str);
    }
    free(node);
}

/**
 * @brief creates AST stack
 * @return created AST stack
 */
ASTStack* CreateStackAST() {
    ASTStack *stack = (ASTStack*)malloc(sizeof(ASTStack));
    if (!stack) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(99);
    }
    
    stack->top = NULL;
    return stack;
}

/**
 * @brief pushs ast node onto stack
 * @param stack AST stack
 * @param node node to be pushed
 */
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

/**
 * @brief pops top from ast stack 
 * @param stack AST stack
 * @return popped node
 */
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

/**
 * @brief frees ast stack
 * @param stack AST stack
 */
void FreeStackAST(ASTStack *stack) {
    if(stack == NULL){
        return;
    }

    StackNode *current = stack->top;
    while (current != NULL) {
        StackNode *next = current->next;
        free(current);
        current = next;
    }
    free(stack);
}