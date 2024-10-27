#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASTNode* CreateAST(){
    return CreateNode(TYPE_PROGRAM);
}

ASTNode* CreateNode(ASTNodeType type){
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if(node == NULL){
        printf("error");
        return NULL;
    }

    node->type = type;

    if(type == TYPE_VALUE){
        node->data.value = 0;
    }else if(type == TYPE_OPERATOR || type == TYPE_REL_OPERATOR){
        node->data.op = OP_DEFAULT;
    }else{
        node->data.str = NULL;
    }
    
    node->left = NULL;
    node->right = NULL;
    return node;
}

ASTNode* CreateCodeNode(ASTNode *node){
    if(node == NULL){
        printf("error");
        return NULL;
    }

    ASTNode *temp = NULL;
    if(node->type == TYPE_FUN_DECL || node->type == TYPE_WHILE){
        if(node->right == NULL){
            node->right = CreateNode(TYPE_CODE);
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
            printf("error");
            return NULL;
        }
    }else{
        if(node->left == NULL){
            node->left = CreateNode(TYPE_CODE);
            return node->left;
        }else{
            temp = node->left;
        }
    }

    while(temp->left != NULL){
        temp = temp->left;
    }

    temp->left = CreateNode(TYPE_CODE);
    return temp->left;
}

ASTNode* CreateVarDeclNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("error");
        return NULL;
    }
    node->right = CreateNode(TYPE_VAR_DECL);
    return node->right;
}

ASTNode* CreateFunDeclNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("error");
        return NULL;
    }
    node->right = CreateNode(TYPE_FUN_DECL);
    return node->right;
}

ASTNode* CreateIdNode(ASTNode *node, char *id){
    if(node == NULL){
        printf("error");
        return NULL;
    }

    if(node->type == TYPE_VAR_DECL || node->type == TYPE_ASSIGNMENT || node->type == TYPE_FUN_DECL || node->type == TYPE_FUN_CALL){
        node->left = CreateNode(TYPE_ID);
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
            node->left->left = CreateNode(TYPE_ID);
            node->left->left->data.str = strdup(id);      
            if(node->left->left->data.str == NULL){
                fprintf(stderr, "AST NODE: Memory allocation failed\n");
                exit(-1);
            }
            return node;
        }
    }else{
        printf("error");
        return NULL;
    }
}

ASTNode* CreateIfElseNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("error");
        return NULL;
    }
    node->right = CreateNode(TYPE_IF_ELSE);
    node->right->right = CreateNode(TYPE_IF_ELSE1);
    node->right->left = CreateNode(TYPE_BLOCK);
    return node->right;
}

ASTNode* CreateIfNode(ASTNode *node){
    if(node->type != TYPE_IF_ELSE){
        printf("error");
        return NULL;
    }
    node->right->left = CreateNode(TYPE_IF);
    return node->right;
}

ASTNode* CreateElseNode(ASTNode *node){
    if(node->type != TYPE_IF_ELSE1){
        printf("error");
        return NULL;
    }
    node->right = CreateNode(TYPE_ELSE);
    return node;
}

ASTNode* CreateWhileNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("error");
        return NULL;
    }
    node->right = CreateNode(TYPE_WHILE);
    node->right->left = CreateNode(TYPE_WHILE1);
    return node->right;
}

ASTNode* CreateReturnNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("error");
        return NULL;
    }
    node->right = CreateNode(TYPE_RETURN);
    return node->right;
}

ASTNode* CreateFunCallNode(ASTNode *node){
    if(node->type != TYPE_CODE || node->right != NULL){
        printf("error");
        return NULL;
    }
    node->right = CreateNode(TYPE_FUN_CALL);
    return node->right;
}

ASTNode* CreateArgumentNode(ASTNode *node, char *id){
    if(node->type != TYPE_FUN_CALL){
        printf("error");
        return NULL;
    }

    ASTNode *temp = node;
    while(temp->right != NULL){
        temp = temp->right;
    }
    temp->right = CreateNode(TYPE_ARGUMENT);
    temp->right->data.str = strdup(id);
    if(temp->right->data.str == NULL){
        fprintf(stderr, "AST NODE: Memory allocation failed\n");
        exit(-1);
    }
    return node;
}

ASTNode* CreateParamNode(ASTNode *node, char *id){
    if(node->type != TYPE_FUN_DECL || node->left == NULL){
        printf("error");
        return NULL;
    }

    ASTNode *temp = node;
    while(temp->left != NULL){
        temp = temp->left;
    }
    temp->left = CreateNode(TYPE_PARAMETER);
    temp->left->data.str = strdup(id);
    if(temp->left->data.str == NULL){
        fprintf(stderr, "AST NODE: Memory allocation failed\n");
        exit(-1);
    }
    return node;
    
}

int main(){
    //test
    ASTNode *ast = CreateAST();
    ASTNode *temp = CreateCodeNode(ast);
    ASTNode *temp1 = CreateCodeNode(ast);
    ASTNode *temp2 = CreateCodeNode(ast);
    ASTNode *temp3 = CreateCodeNode(temp1);
    temp3 = CreateIfElseNode(temp1);
    temp3 = CreateIdNode(temp3, "a");
    temp3 = CreateFunCallNode(temp);
    temp3 = CreateArgumentNode(temp3, "a");
    temp3 = CreateArgumentNode(temp3, "b");
    temp3 = CreateArgumentNode(temp3, "c");
    temp3 = CreateFunDeclNode(temp2);
    temp3 = CreateIdNode(temp3, "aaa");
    temp3 = CreateParamNode(temp3, "ab");
    temp3 = CreateParamNode(temp3, "abc");
}