/**
 * @file parser.c
 * @author Martin Vrablec
 * @brief  source file for top down parser, gets token list checks syntax using LL grammar and builds AST
 * @todo
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "parser.h"
#include "token.h"
#include "symtable.h"
#include "ast.h"
#include "error.h"
#include "expr.h"

NonTerminal lastNonTerminal = NON_TERMINAL_UNKOWN;
ASTNode* temp = NULL;

/**
 * @brief Function for parsing token list, checks error codes of grammar rules, expresion parser. Builds AST
 * @param tree pointer to AST
 * @param list pointer to token list
 * @return error parser if token isnt based on ll grammar rules
 */
int Parser(ASTNode** tree, TokenList* list)
{
    int errorExpr = 0;
    Token *saveToken = InitToken();
    Stack* stack = InitStack();
    Error error = 0;

    // Create base ast
    ASTNode *ast = CreateAST(); 
    ASTNode *root = ast;
    CreateCodeNode(ast);
    ASTNode *expr_root = NULL;
    Tokentype lastInteresingToken = TOKEN_UNKNOWN;

    // Push start item to stack
    PushItem(stack, TOKEN_UNKNOWN, NON_T_BODY);

    // looping until all rules are resolved
    while (!Empty(stack))
    {
        // skipps whitespaces, comments and new lines
        if(list->currToken->type == TOKEN_SPACE || list->currToken->type == TOKEN_COMMENT || list->currToken->type == TOKEN_EOL)
        {
            Token* nextToken = list->currToken->nextToken;
            list->currToken = nextToken;
            continue;
        }

        // top of stack is token so there needs to be checked what rule will follow
        if(Top(stack)->nonTerminal != NON_TERMINAL_UNKOWN)
        {
            error = LLGrammar(stack, list->currToken->type);
            if(error != 0)
            {
                return ERROR_PARSER;
            }

        }
        Top(stack);

        // top of the stack is nonterminal expression so we need to call expression parser
        if(Top(stack)->nonTerminal == NON_T_EXPR) 
        {
            // pop  non terminal from stack
            Pop(stack);
            lastNonTerminal = NON_T_EXPR;
            errorExpr = expr_start(&expr_root, &list, Top(stack)->tokenType);
            if (errorExpr != 0) {
                return ERROR_PARSER;
            }
        }

        // top of the stack is token we need to check if it is same as current token
        if(Top(stack)->tokenType != list->currToken->type)
        {
            return ERROR_PARSER;
        }

        Pop(stack);

        // if token is interesting save it
        if(InterestingTokens(list->currToken->type))
        {
            lastInteresingToken = list->currToken->type;
        }

        // based on last interesting token build AST
        if (lastInteresingToken != TOKEN_UNKNOWN )
        {
            // somwhow works dont change
            if(lastInteresingToken == TOKEN_CURLY_RIGHT_PAR && list->currToken->type == TOKEN_CURLY_RIGHT_PAR)
            {
                BuildAST(&expr_root, &ast, lastInteresingToken, list->currToken, saveToken);
                ast = root;
            }
            else if(lastInteresingToken != TOKEN_CURLY_RIGHT_PAR)
            {
                BuildAST(&expr_root, &ast, lastInteresingToken, list->currToken, saveToken);
                ast = root;
            }
        }

        // clear expression root for another expression
        expr_root = NULL;

        //continue to next token
        list->currToken = list->currToken->nextToken;

    }
    FreeStack(stack);

    *tree = ast;

    return 0;
}

/**
 * @brief calls functions which push nonterminal and tokens to stack based on ll grammar rules 
 * @param stack pointer to stack
 * @param type type of token
 * @return error parser if token isnt based on ll grammar rules
 */
int LLGrammar(Stack* stack, Tokentype type)
{
    NonTerminal nonterminal = Top(stack)->nonTerminal;
    Error error = 0;

    // gets nonterminal and calls functions based on that nonterminal until top of the stack is token or nonterminal expr
    // if tokenType is unknown it means that it is nonterminal
    while (Top(stack)->tokenType == TOKEN_UNKNOWN && Top(stack)->nonTerminal != NON_T_EXPR)
    {
        lastNonTerminal = nonterminal;

        Pop(stack);
        switch (nonterminal)
        {
            case NON_T_BODY:
                error = NonTerminalBodyPush(stack, type);
            break;
            case NON_T_AFTER_BODY:
                error = NonTerminalAfterBodyPush(stack, type);
            break;
            case NON_T_FUNCTION_BODY:
                error = NonTerminalFunctionBodyPush(stack, type);
            break;
            case NON_T_ID_HELPER:
                error = NonTerminalIdHelper(stack, type);
            break;
            case NON_T_ID_CONTINUE:
                error = NonTerminalIdContinuePush(stack, type);
            break;
            case NON_T_PARAMS_ENTER:
                error = NonTerminalParamsEnterPush(stack, type);
            break;
            case NON_T_DECLARATION_CONTINUE:
                error = NonTerminalDeclarationContinuePush(stack, type);
            break;
            case NON_T_NEXT_PARAMS_ENTER:
                error = NonTerminalNextParamsEnterPush(stack, type);
            break;
            case NON_T_VAR_OR_CONST:
                error = NonTerminalVarOrConstPush(stack, type);
            break;
            case NON_T_BARS:
                error = NonTerminalBarsPush(stack, type);
            break;
            case NON_T_PARAMS:
                error = NonTerminalParamsPush(stack, type);
            break;
            case NON_T_NEXT_PARAMS:
                error = NonTerminalNextParamsPush(stack, type);
            break;
            case NON_T_RETURN_TYPE:
                error = NonTerminalReturnTypePush(stack, type);
            break;
            case NON_T_TYPE:
                error = NonTerminalTypePush(stack, type);
            break;
            case NON_T_TERM:
                error = NonTerminalTermPush(stack, type);
            break;
            default:
            break;
        }
        if (error != 0)
        {
            return error;
        }
        nonterminal = Top(stack)->nonTerminal;
        
    }

    return error;
}

/**
 * @brief Function for pushing items to stack for non terminal <body>
 * @param stack pointer to stack
 * @param type type of token
 * @return error parser if token isnt based on ll grammar rules
 */
int NonTerminalBodyPush(Stack* stack,Tokentype type)
{
    // <body> -> CONST ID = @IMPORT(STRING); <after_body>
    if(type == TOKEN_const)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_AFTER_BODY);
        PushItem(stack, TOKEN_SEMICOLON, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_STRING, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_PROLOG, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_ASSIGN, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_VARIABLE, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_const, NON_TERMINAL_UNKOWN);
    }
    else
    {
        return ERROR_PARSER;
    }
    return 0;
}

/**
 * @brief Function for pushing items to stack for non terminal <after_body>
 * @param stack pointer to stack
 * @param type type of token
 * @return error parser if token isnt based on ll grammar rules
 */
int NonTerminalAfterBodyPush(Stack* stack, Tokentype type)
{
    // <after_body> -> PUB FN ID (<params>) <return_type> { <function_body> } <after_body>
    if(type == TOKEN_pub)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_AFTER_BODY);
        PushItem(stack, TOKEN_CURLY_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_CURLY_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_RETURN_TYPE);
        PushItem(stack, TOKEN_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_PARAMS);
        PushItem(stack, TOKEN_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_VARIABLE, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_fn, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_pub, NON_TERMINAL_UNKOWN);
    }
    // <after_body> -> EOF
    else if (type == TOKEN_EOF)
    {
        PushItem(stack, TOKEN_EOF, NON_TERMINAL_UNKOWN);
    }
    else
    {
        return ERROR_PARSER;
    }
    return 0;
}

/**
 * @brief Function for pushing items to stack for non terminal <function_body>
 * @param stack pointer to stack
 * @param type type of token
 * @return 0 (because there can be epsilon) 
 */
int NonTerminalFunctionBodyPush(Stack* stack, Tokentype type)
{
    // <function_body> -> <var_or_const> ID <declaration_continue>; <function_body>
    if(type == TOKEN_const || type == TOKEN_var)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_SEMICOLON, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_DECLARATION_CONTINUE);
        PushItem(stack, TOKEN_VARIABLE, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_VAR_OR_CONST);
    }
    //<function_body> -> <id_helper>
    else if(type == TOKEN_VARIABLE || type == TOKEN_UNDERSCORE)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_ID_HELPER);
    }
    //<function_body> -> IF (expression) <bars> <eol> {<function_body>} ELSE {<function_body>} <function_body>
    else if(type == TOKEN_if)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_CURLY_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_CURLY_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_else, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_CURLY_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_CURLY_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_BARS);
        PushItem(stack, TOKEN_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_EXPR); 
        PushItem(stack, TOKEN_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_if, NON_TERMINAL_UNKOWN);
    }
    //<function_body> -> WHILE (expression) <bars> {<function_body>} <function_body>
    else if(type == TOKEN_while)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_CURLY_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_CURLY_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_BARS);
        PushItem(stack, TOKEN_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_EXPR); 
        PushItem(stack, TOKEN_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_while, NON_TERMINAL_UNKOWN);
    }
    //<function_body> -> RETURN expression; <function_body>
    else if(type == TOKEN_return)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_SEMICOLON, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_EXPR); 
        PushItem(stack, TOKEN_return, NON_TERMINAL_UNKOWN);
    }
    //<function_body> -> ε
    return 0;

}

/**
 * @brief Function for pushing items to stack for non terminal <dec
 * @param stack pointer to stack
 * @param type type of token
 * @return error parser if token isnt based on ll grammar rules
 */
int NonTerminalDeclarationContinuePush(Stack* stack, Tokentype type)
{
    // <declaration_continue> -> : <type> = expression
    if(type == TOKEN_COLON)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_EXPR); 
        PushItem(stack, TOKEN_ASSIGN, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_TYPE);
        PushItem(stack, TOKEN_COLON, NON_TERMINAL_UNKOWN);
    }
    // <declaration_continue> -> = expression
    else if(type == TOKEN_ASSIGN)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_EXPR); 
        PushItem(stack, TOKEN_ASSIGN, NON_TERMINAL_UNKOWN);
    }
    else
    {
        return ERROR_PARSER;
    }
    return 0;
}

/**
 * @brief Function for pushing items to stack for non terminal <id_helper>
 * @param stack pointer to stack
 * @param type type of token
 * @return error parser if token isnt based on ll grammar rules
 */
int NonTerminalIdHelper(Stack* stack, Tokentype type)
{
    // <id_helper> -> ID <id_continue>;  <function_body>
    if(type == TOKEN_VARIABLE)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_SEMICOLON, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_ID_CONTINUE);
        PushItem(stack, TOKEN_VARIABLE, NON_TERMINAL_UNKOWN);
    }
    //<id_helper> -> _ = expression;  <function_body>
    else if(type == TOKEN_UNDERSCORE)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_SEMICOLON, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_EXPR); 
        PushItem(stack, TOKEN_ASSIGN, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNDERSCORE, NON_TERMINAL_UNKOWN);
    }
    else
    {
        return ERROR_PARSER;
    }
    return 0;
}

/**
 * @brief Function for pushing items to stack for non terminal <id_continue>
 * @param stack pointer to stack
 * @param type type of token
 * @return error parser if token isnt based on ll grammar rules
 */
int NonTerminalIdContinuePush(Stack* stack, Tokentype type)
{
    // <id_continue> -> = expression
    if(type == TOKEN_ASSIGN)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_EXPR); 
        PushItem(stack, TOKEN_ASSIGN, NON_TERMINAL_UNKOWN);
    }
    // <id_continue> -> .ID(<arguments>)
    else if(type == TOKEN_DOT)
    {
        PushItem(stack, TOKEN_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_PARAMS_ENTER);
        PushItem(stack, TOKEN_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_VARIABLE, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_DOT, NON_TERMINAL_UNKOWN);
    }
    // <id_continue> -> (<arguments>)
    else if(type == TOKEN_LEFT_PAR)
    {
        PushItem(stack, TOKEN_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_PARAMS_ENTER);
        PushItem(stack, TOKEN_LEFT_PAR, NON_TERMINAL_UNKOWN);
    }
    else
    {
        return ERROR_PARSER;
    }
    return 0;
}

/**
 * @brief Function for pushing items to stack for non terminal <arguments>
 * @param stack pointer to stack
 * @param type type of token
 * @return 0 (because there can be epsilon) 
 */
int NonTerminalParamsEnterPush(Stack* stack, Tokentype type)
{
    // <arguments> -> <term> <next_arguments>
    if(type == TOKEN_INTEGER || type == TOKEN_FLOAT || type == TOKEN_STRING || type == TOKEN_VARIABLE || type == TOKEN_null)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_NEXT_PARAMS_ENTER);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_TERM);
    }
    // <arguments> -> ε
    return 0;
}

/**
 * @brief Function for pushing items to stack for non terminal <next_arguments>
 * @param stack pointer to stack
 * @param type type of token
 * @return 0 (because there can be epsilon) 
 */
int NonTerminalNextParamsEnterPush(Stack* stack, Tokentype type)
{
    // <next_arguments> -> , <arguments>
    if(type == TOKEN_COMMA)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_PARAMS_ENTER);
        PushItem(stack, TOKEN_COMMA, NON_TERMINAL_UNKOWN);
    }
    // <next_arguments> -> ε
    return 0;
}

/**
 * @brief Function for pushing items to stack for non terminal <var_or_const>
 * @param stack pointer to stack
 * @param type type of token
 * @return error parser if token isnt based on ll grammar rules
 */
int NonTerminalVarOrConstPush(Stack* stack, Tokentype type)
{
    // <var_or_const> -> VAR
    if(type == TOKEN_var)
    {
        PushItem(stack, TOKEN_var, NON_TERMINAL_UNKOWN);
    }
    // <var_or_const> -> CONST
    else if(type == TOKEN_const)
    {
        PushItem(stack, TOKEN_const, NON_TERMINAL_UNKOWN);
    }
    else
    {
        return ERROR_PARSER;
    }
    return 0;
}

/**
 * @brief Function for pushing items to stack for non terminal <params>
 * @param stack pointer to stack
 * @param type type of token
 * @return 0 (because there can be epsilon) 
 */
int NonTerminalParamsPush(Stack* stack, Tokentype type)
{
    // <params> -> ID : <type> <next_params>
    if(type == TOKEN_VARIABLE)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_NEXT_PARAMS);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_TYPE);
        PushItem(stack, TOKEN_COLON, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_VARIABLE, NON_TERMINAL_UNKOWN);
    }
    // <params> := ε

    return 0;
}

/**
 * @brief Function for pushing items to stack for non terminal <next_params>
 * @param stack pointer to stack
 * @param type type of token
 * @return 0 (because there can be epsilon) 
 */
int NonTerminalNextParamsPush(Stack* stack, Tokentype type)
{
    // <next_params> -> , <params>
    if(type == TOKEN_COMMA)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_PARAMS);
        PushItem(stack, TOKEN_COMMA, NON_TERMINAL_UNKOWN);
    }
    // <next_params> -> ε

    return 0;
}

/**
 * @brief Function for pushing items to stack for non terminal <bars>
 * @param stack pointer to stack
 * @param type type of token
 * @return 0 (because there can be epsilon) 
 */
int NonTerminalBarsPush(Stack* stack, Tokentype type)
{
    // <bars> -> |ID|
    if(type == TOKEN_BAR)
    {
        PushItem(stack, TOKEN_BAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_VARIABLE, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_BAR, NON_TERMINAL_UNKOWN);
    }
    // <bars> -> ε

    return 0;
}

/**
 * @brief Function for pushing items to stack for non terminal <return_type>
 * @param stack pointer to stack
 * @param type type of token
 * @return error parser if token isnt based on ll grammar rules
 */
int NonTerminalReturnTypePush(Stack* stack, Tokentype type)
{
    // <return_type> -> <type>
    if(type == TOKEN_i32 || type == TOKEN_f64 || type == TOKEN_u8 || type == TOKEN_i32_NULL || type == TOKEN_f64_NULL || type == TOKEN_u8_NULL)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_TYPE);
    }
    // <return_type> -> VOID
    else if(type == TOKEN_void)
    {
        PushItem(stack, TOKEN_void, NON_TERMINAL_UNKOWN);
    }
    else
    {
        return ERROR_PARSER;
    }
    return 0;
}

/**
 * @brief Function for pushing items to stack for non terminal <type>
 * @param stack pointer to stack
 * @param type type of token
 * @return error parser if token isnt based on ll grammar rules
 */
int NonTerminalTypePush(Stack* stack, Tokentype type)
{
    // <type> -> i32
    if(type == TOKEN_i32)
    {
        PushItem(stack, TOKEN_i32, NON_TERMINAL_UNKOWN);
    }
    // <type> -> f64
    else if(type == TOKEN_f64)
    {
        PushItem(stack, TOKEN_f64, NON_TERMINAL_UNKOWN);
    }
    // <type> -> u8
    else if(type == TOKEN_u8)
    {
        PushItem(stack, TOKEN_u8, NON_TERMINAL_UNKOWN);
    }
    else if(type == TOKEN_i32_NULL)
    {
        PushItem(stack, TOKEN_i32_NULL, NON_TERMINAL_UNKOWN);
    }
    else if(type == TOKEN_f64_NULL)
    {
        PushItem(stack, TOKEN_f64_NULL, NON_TERMINAL_UNKOWN);
    }
    else if(type == TOKEN_u8_NULL)
    {
        PushItem(stack, TOKEN_u8_NULL, NON_TERMINAL_UNKOWN);
    }
    else
    {
        return ERROR_PARSER;
    }
    return 0;
}

/**
 * @brief Function for pushing items to stack for non terminal <term>
 * @param stack pointer to stack
 * @param type type of token
 * @return error parser if token isnt based on ll grammar rules
 */
int NonTerminalTermPush(Stack* stack, Tokentype type)
{
    // <term> -> ID
    if(type == TOKEN_VARIABLE)
    {
        PushItem(stack, TOKEN_VARIABLE, NON_TERMINAL_UNKOWN);
    }
    // <term> -> VALUE_FLOAT
    else if(type == TOKEN_FLOAT)
    {
        PushItem(stack, TOKEN_FLOAT, NON_TERMINAL_UNKOWN);
    }
    // <term> -> VALUE_INT
    else if(type == TOKEN_INTEGER)
    {
        PushItem(stack, TOKEN_INTEGER, NON_TERMINAL_UNKOWN);
    }
    // <term> -> VALUE_STRING
    else if(type == TOKEN_STRING)
    {
        PushItem(stack, TOKEN_STRING, NON_TERMINAL_UNKOWN);
    }
    // <term> -> NULL
    else if(type == TOKEN_null)
    {
        PushItem(stack, TOKEN_null, NON_TERMINAL_UNKOWN);
    }
    else
    {;
        return ERROR_PARSER;
    }
    return 0;
}


// --------------------------------- AST CREATION ---------------------------------

/**
 * @brief finds deepest code node in ast tree
 * @param ast pointer to AST
 * @return pointer to deepest code node
 */
ASTNode* findDeepestCodeNode(ASTNode** ast) 
{
    if ((*ast)->left == NULL) {
        return *ast;
    } else {
        return findDeepestCodeNode(&(*ast)->left);
    }
}

/**
 * @brief finds deepest function declaration node in ast tree
 * @param ast pointer to AST
 * @return pointer to deepest function declaration node
 */
ASTNode* findDeepestFunDecNode(ASTNode** ast)
{
    *ast = findDeepestCodeNode(&(*ast));

    // check if current code node has function declaration 
    return (*ast)->right  == NULL ? *ast : (*ast)->right;
}



/**
 * @brief finds deepest param node in function declaration node in ast tree
 * @param ast pointer to AST
 * @return pointer to deepest param node
 */
ASTNode* findDeepestParamNode(ASTNode** ast)
{
    *ast = findDeepestFunDecNode(&(*ast));
    if((*ast)->left == NULL)
    {
        return *ast;
    }
    else
    {
        return findDeepestParamNode(&(*ast)->left);
    }
}

/**
 * @brief finds deepest code node in functions is used as helper function for findDeepestFunctionBodyNode
 * @param ast pointer to AST
 * @return pointer to deepest function body node
 */
ASTNode* findDeepestFuncCodeNode(ASTNode** ast)
{
    // special types of traversing through AST because of nested if else and while
    // based on how we agreed to create AST
    if ((*ast)->left == NULL && (*ast)->right->type == TYPE_WHILE)
    {

        return (*ast)->right->right == NULL ? (*ast)->right : findDeepestFuncCodeNode(&(*ast)->right->right);
    } 
    else if((*ast)->left == NULL && (*ast)->right->type == TYPE_IF_ELSE && (*ast)->right->right->left->type == TYPE_IF) 
    {
        return (*ast)->right->right->left->left == NULL ? (*ast)->right->right->left : findDeepestFuncCodeNode(&(*ast)->right->right->left->left);
    }
    else if((*ast)->left == NULL && (*ast)->right->type == TYPE_IF_ELSE && (*ast)->right->right->right != NULL) 
    {
        if((*ast)->right->right->right->type == TYPE_ELSE) 
        {
            return (*ast)->right->right->right->left == NULL ? (*ast)->right->right->right : findDeepestWhileNodeHelp(&(*ast)->right->right->right->left);
        }
    }
    else if ((*ast)->left != NULL)
    {
        return findDeepestFuncCodeNode(&(*ast)->left);
    } 
    return *ast;
}

/**
 * @brief finds deepest code node in functions in ast tree, used in various functions for finding deepest nodes
 * @param ast pointer to AST
 * @return pointer to deepest function body node
 */
ASTNode* findDeepestFunctionBodyNode(ASTNode** ast)
{
    *ast = findDeepestFunDecNode(&(*ast));
    if((*ast)->right == NULL)
    {
        return *ast;
    }
    else
    {
        *ast = (*ast)->right;
        if ((*ast)->left == NULL && ((*ast)->right->type == TYPE_WHILE || (*ast)->right->type == TYPE_IF_ELSE ) ) return findDeepestFuncCodeNode(&(*ast));
        else if ((*ast)->left != NULL) {
            return findDeepestFuncCodeNode(&(*ast)->left);
        }
        return *ast;
    }
}

/**
 * @brief finds deepest var node in functions in ast tree
 * @param ast pointer to AST
 * @return pointer to deepest var node
 */
ASTNode* findDeepestVarNode(ASTNode** ast)
{
    *ast = findDeepestFunctionBodyNode(&(*ast));
    if((*ast)->right == NULL)
    {
        return *ast;
    }
    else if((*ast)->right->type != TYPE_VAR_DECL)
    {
        return *ast;
    }
    else
    {
        *ast = (*ast)->right;
        return *ast;
    }
}

/**
 * @brief finds deepest assignment node in functions in ast tree
 * @param ast pointer to AST
 * @return pointer to deepest assignment node
 */
ASTNode* findDeepestAssignmentNode(ASTNode** ast)
{
    *ast = findDeepestFunctionBodyNode(&(*ast));
    if((*ast)->right == NULL)
    {
        return *ast;
    }
    else if((*ast)->right->type != TYPE_ASSIGNMENT)
    {
        return *ast;
    }
    else
    {
        *ast = (*ast)->right;
        return *ast;
    }
}

/**
 * @brief finds deepest const node in functions in ast tree
 * @param ast pointer to AST
 * @return pointer to deepest const node
 */
ASTNode* findDeepestConstNode(ASTNode** ast)
{
    *ast = findDeepestFunctionBodyNode(&(*ast));
    if((*ast)->right == NULL)
    {
        return *ast;
    }
    else if((*ast)->right->type != TYPE_CON_DECL)
    {
        return *ast;
    }
    else
    {
        *ast = (*ast)->right;
        return *ast;
    }
}

/**
 * @brief finds deepest return node in functions in ast tree
 * @param ast pointer to AST
 * @return pointer to deepest return node
 */
ASTNode* findDeepestReturnNode(ASTNode** ast)
{
    *ast = findDeepestFunctionBodyNode(&(*ast));
    if((*ast)->right == NULL)
    {
        return *ast;
    }
    else if((*ast)->right->type != TYPE_RETURN)
    {
        return *ast;
    }
    else
    {
        *ast = (*ast)->right;
        return *ast;
    }
}

/**
 * @brief finds deepest if node in functions in ast tree, helper function to findDeepestIfElseNode
 * @param ast pointer to AST
 * @return pointer to deepest if node
 */
ASTNode* findDeepestIfElseNodeHelp(ASTNode** ast)
{
    // special types of traversing through AST because of nested if else and while
    // based on how we agreed to create AST
    if ((*ast)->left == NULL && (*ast)->right->type == TYPE_WHILE)
    {
        temp = (*ast)->right;
        return (*ast)->right->right == NULL ? temp : findDeepestIfElseNodeHelp(&(*ast)->right->right);
    } 
    else if((*ast)->left == NULL && (*ast)->right->type == TYPE_IF_ELSE && (*ast)->right->right->left->type == TYPE_IF) 
    {
        temp =(*ast)->right;
        return (*ast)->right->right->left->left == NULL ? temp : findDeepestIfElseNodeHelp(&(*ast)->right->right->left->left);
    }
    else if((*ast)->left == NULL && (*ast)->right->type == TYPE_IF_ELSE && (*ast)->right->right->right != NULL) 
    {
        if((*ast)->right->right->right->type == TYPE_ELSE) 
        {
            temp = (*ast)->right;
            return (*ast)->right->right->right->left == NULL ? temp : findDeepestIfElseNodeHelp(&(*ast)->right->right->right->left);
        }
    }
    else if ((*ast)->left != NULL)
    {
        return findDeepestIfElseNodeHelp(&(*ast)->left);
    } 
    return *ast;
}

/**
 * @brief finds deepest if else node in functions in ast tree
 * @param ast pointer to AST
 * @return pointer to deepest if else node
 */
ASTNode* findDeepestIfElseNode(ASTNode** ast) {

    *ast = findDeepestFunDecNode(&(*ast));
    if((*ast)->right == NULL)
    {
        return *ast;
    }
    else
    {
        *ast = (*ast)->right;
        if ((*ast)->left == NULL && ((*ast)->right->type == TYPE_WHILE || (*ast)->right->type == TYPE_IF_ELSE )) return findDeepestIfElseNodeHelp(&(*ast));
        else if ((*ast)->left != NULL) {
            return findDeepestIfElseNodeHelp(&(*ast)->left);
        }
        return NULL;
    }
}

/**
 * @brief finds deepest non closed node, used as helper function for findDeepestWhileNode
 * @param ast pointer to AST
 * @return pointer to deepest non closed node
 */
ASTNode* findDeepestWhileNodeHelp(ASTNode** ast) {
    if ((*ast)->left == NULL && (*ast)->right->type == TYPE_WHILE)
    {
        temp = (*ast)->right;
        return (*ast)->right->right == NULL ? temp : findDeepestWhileNodeHelp(&(*ast)->right->right);
    } 
    else if((*ast)->left == NULL && (*ast)->right->type == TYPE_IF_ELSE && (*ast)->right->right->left->type == TYPE_IF) 
    {
        temp =(*ast)->right->right->left;
        return (*ast)->right->right->left->left == NULL ? temp : findDeepestWhileNodeHelp(&(*ast)->right->right->left->left);
    }
    else if((*ast)->left == NULL && (*ast)->right->type == TYPE_IF_ELSE && (*ast)->right->right->right != NULL) 
    {
        if((*ast)->right->right->right->type == TYPE_ELSE) 
        {
            temp = (*ast)->right->right->right;
            return (*ast)->right->right->right->left == NULL ? temp : findDeepestWhileNodeHelp(&(*ast)->right->right->right->left);
        }
    }
    else if ((*ast)->left != NULL)
    {
        return findDeepestWhileNodeHelp(&(*ast)->left);
    } 
    return *ast;
}

/**
 * @brief finds deepest non closed node (if, else, while)
 * @param ast pointer to AST
 * @return pointer to deepest non closed node
 */
ASTNode* findDeepestWhileNode(ASTNode** ast) {

    *ast = findDeepestFunDecNode(&(*ast));
    if((*ast)->right == NULL)
    {
        return *ast;
    }
    else
    {
        *ast = (*ast)->right;
        if ((*ast)->left == NULL && ((*ast)->right->type == TYPE_WHILE || (*ast)->right->type == TYPE_IF_ELSE )) return findDeepestWhileNodeHelp(&(*ast));
        else if ((*ast)->left != NULL) {
            return findDeepestWhileNodeHelp(&(*ast)->left);
        }
        return NULL;
    }
}

/**
 * @brief finds first closed if node in functions in ast tree, helpser function for findFirstClosedIfNode
 * @param ast pointer to AST
 * @return pointer to first closed if node
 */
ASTNode* findFirstClosedIfNodeHelp(ASTNode** ast) {
    if ((*ast)->left == NULL && (*ast)->right->type == TYPE_WHILE)
    {
        return (*ast)->right->right == NULL ? NULL : findFirstClosedIfNodeHelp(&(*ast)->right->right);
    } 
    else if((*ast)->left == NULL && (*ast)->right->type == TYPE_IF_ELSE) 
    {
        if((*ast)->right->right->left->type == TYPE_IF_CLOSED) 
        {
            return (*ast)->right;
        }
        return (*ast)->right->right->left->left == NULL ? NULL  : findFirstClosedIfNodeHelp(&(*ast)->right->right->left->left);
    }
    else if ((*ast)->left != NULL)
    {
        return findFirstClosedIfNodeHelp(&(*ast)->left);
    } 
    else
    {
        return NULL;
    }
}

/**
 * @brief finds first closed if node in functions in ast tree
 * @param ast pointer to AST
 * @return pointer to first closed if node
 */
ASTNode* findFirstClosedIfNode(ASTNode** ast) {

    *ast = findDeepestFunDecNode(&(*ast));
    if((*ast)->right == NULL)
    {
        return *ast;
    }
    else
    {
        *ast = (*ast)->right;
        if ((*ast)->left == NULL && ((*ast)->right->type == TYPE_WHILE || (*ast)->right->type == TYPE_IF_ELSE )) return findFirstClosedIfNodeHelp(&(*ast));
        else if ((*ast)->left != NULL) {
            return findFirstClosedIfNodeHelp(&(*ast)->left);
        }
        return NULL;
    }
}

/**
 * @brief main function for building AST, based on token types and AST nodes
 * @param expr_root pointer to expression root, used only for expressions
 * @param ast pointer to AST, used for creating AST
 * @param interestingToken token type
 * @param token pointer to token
 * @param saveToken pointer to token, used for saving names of functions or variables for later use in assignment or funccalls
 */
void BuildAST(ASTNode** expr_root, ASTNode** ast, Tokentype interestingToken, Token* token, Token* saveToken)
{
    switch (interestingToken)
    {
        case TOKEN_fn:
            if(token->type == TOKEN_fn)
            {
                *ast = findDeepestCodeNode(&(*ast));
                if((*ast)->right != NULL)
                {
                    *ast = CreateCodeNode(*ast);
                }
                *ast = CreateFunDeclNode(*ast);
            }
            else if(token->type == TOKEN_VARIABLE)
            {
                *ast = findDeepestFunDecNode(&(*ast));
                // function id 
                if((*ast)->left == NULL)
                {
                    *ast = CreateIdNode(*ast, token->data);
                }
                // param id
                else
                {
                    *ast = CreateParamNode(*ast, token->data);
                }
            }
            // return type
            else if(token->type == TOKEN_void || token->type == TOKEN_i32 || token->type == TOKEN_f64 || token->type == TOKEN_u8 || token->type == TOKEN_i32_NULL || token->type == TOKEN_f64_NULL || token->type == TOKEN_u8_NULL)
            {
                *ast = findDeepestFunDecNode(&(*ast));
                *ast = CreateTypeNode(*ast, TokenTypeToDataType(token->type));
            }
        break;
        case TOKEN_const:
            // dont check const for prolog 
            if(lastNonTerminal == NON_T_BODY){
                break;
            }
            if(token->type == TOKEN_const)
            {
                *ast = findDeepestFunctionBodyNode(&(*ast));
                *ast = CreateCodeNode(*ast);
                *ast = CreateConDeclNode(*ast);
            }
            // id 
            else if(token->type == TOKEN_VARIABLE)
            {
                *ast = findDeepestConstNode(&(*ast));
                *ast = CreateIdNode(*ast, token->data);
            }
            else if(token->type == TOKEN_i32 || token->type == TOKEN_f64 || token->type == TOKEN_u8 || token->type == TOKEN_i32_NULL || token->type == TOKEN_f64_NULL || token->type == TOKEN_u8_NULL)
            {
                *ast = findDeepestConstNode(&(*ast));
                *ast = CreateTypeNode(*ast, TokenTypeToDataType(token->type));
            }
            // join expression
            else if(lastNonTerminal == NON_T_EXPR)
            {
                *ast = findDeepestConstNode(&(*ast));
                (*ast)->right = *expr_root;
            }
            break;
        case TOKEN_var:
            if(token->type == TOKEN_var)
            {
                *ast = findDeepestFunctionBodyNode(&(*ast));
                *ast = CreateCodeNode(*ast);
                *ast = CreateVarDeclNode(*ast);
            }
            // id
            else if(token->type == TOKEN_VARIABLE)
            {
                *ast = findDeepestVarNode(&(*ast));
                *ast = CreateIdNode(*ast, token->data);
            }
            else if(token->type == TOKEN_i32 || token->type == TOKEN_f64 || token->type == TOKEN_u8 || token->type == TOKEN_i32_NULL || token->type == TOKEN_f64_NULL || token->type == TOKEN_u8_NULL)
            {
                *ast = findDeepestVarNode(&(*ast));
                *ast = CreateTypeNode(*ast, TokenTypeToDataType(token->type));
            }
            // join expression
            else if(lastNonTerminal == NON_T_EXPR)
            {
                *ast = findDeepestVarNode(&(*ast));
                (*ast)->right = *expr_root;
            }
            break;
        case TOKEN_return:
             if(token->type == TOKEN_return)
            {
                *ast = findDeepestFunctionBodyNode(&(*ast));
                *ast = CreateCodeNode(*ast);
                *ast = CreateReturnNode(*ast);
            }
            // join expression
            else if(lastNonTerminal == NON_T_EXPR)
            {
                *ast = findDeepestReturnNode(&(*ast));
                (*ast)->right = *expr_root;
            }
            break;
        case TOKEN_if:
            // creates needed nodes for if else
            if(token->type == TOKEN_if)
            {
                *ast = findDeepestFunctionBodyNode(&(*ast));
                *ast = CreateCodeNode(*ast);
                *ast = CreateIfElseNode(*ast);
                *ast = CreateIfNode(*ast);
            }
            else if (token->type == TOKEN_VARIABLE)
            {
                *ast = findDeepestIfElseNode(&(*ast));
                CreateIdNode(*ast, token->data);
            }
            else if(lastNonTerminal == NON_T_EXPR)
            {
                *ast = findDeepestIfElseNode(&(*ast));
                (*ast)->left->right = *expr_root;
            }
            break;
        case TOKEN_while:
            // creates needed nodes for while
            if(token->type == TOKEN_while)
            {
                *ast = findDeepestFunctionBodyNode(&(*ast));
                *ast = CreateCodeNode(*ast);
                *ast = CreateWhileNode(*ast);
            }
            else if (token->type == TOKEN_VARIABLE)
            {
                *ast = findDeepestFunctionBodyNode(&(*ast));
                CreateIdNode(*ast, token->data);
            }
            else if(lastNonTerminal == NON_T_EXPR)
            {
                *ast = findDeepestFunctionBodyNode(&(*ast));
                (*ast)->left->right = *expr_root;
            }
            break;
        case TOKEN_else:
            // first closed if means that else is going to start 
            *ast = findFirstClosedIfNode(&(*ast));
            if(*ast != NULL)
            {
                *ast = CreateElseNode(*ast);
            }
            break;
        case TOKEN_CURLY_RIGHT_PAR:
            // finds deepests possibly nested if else or while and closes it 
            *ast = findDeepestWhileNode(&(*ast));
            *ast = temp;
            if(*ast != NULL)
            {
                if((*ast)->type == TYPE_IF)
                {
                    (*ast)->type = TYPE_IF_CLOSED;
                }
                else if((*ast)->type == TYPE_WHILE)
                {
                    (*ast)->type = TYPE_WHILE_CLOSED;
                }
                else if((*ast)->type == TYPE_ELSE)
                {
                    (*ast)->type = TYPE_ELSE_CLOSED;
                }
            }
            break;
        case TOKEN_VARIABLE:
        case TOKEN_UNDERSCORE:
            // save token name for id
            if(lastNonTerminal == NON_T_ID_HELPER && interestingToken != TOKEN_UNDERSCORE)
            {
                saveToken->data = strdup(token->data);
                saveToken->type = token->type;
                saveToken->dataLength = token->dataLength;
            }
            // create needed nodes for function call
            else if(token->type == TOKEN_LEFT_PAR)
            {
                *ast = findDeepestFunctionBodyNode(&(*ast));
                *ast = CreateCodeNode(*ast); 
                *ast = CreateFunCallNode(*ast);
                *ast = CreateIdNode(*ast, saveToken->data);
                saveToken = NULL;
            }
            // create needed nodes for parameters for functoin calls
            else if (lastNonTerminal == NON_T_TERM)
            {
                *ast = findDeepestFunctionBodyNode(&(*ast));
                if (token->type == TOKEN_INTEGER)
                {
                    *ast = (*ast)->right;
                    *ast = CreateArgumentNodeI32(*ast, atoi(token->data));
                }
                else if (token->type == TOKEN_FLOAT)
                {
                    *ast = (*ast)->right;
                    *ast = CreateArgumentNodeF64(*ast, atof(token->data));
                }
                else if (token->type == TOKEN_STRING)
                {
                    *ast = (*ast)->right;
                    *ast = CreateArgumentNodeU8(*ast, token->data);
                }
                else if (token->type == TOKEN_VARIABLE)
                {
                    *ast = (*ast)->right;
                    *ast = CreateArgumentNode(*ast, token->data);
                }
                else if (token->type == TOKEN_null)
                {
                    *ast = (*ast)->right;
                    *ast = CreateArgumentNullNode(*ast);
                }
                
                saveToken = NULL;
            }
            // create needed nodes for assignment
            else if(token->type == TOKEN_ASSIGN)
            {
                *ast = findDeepestFunctionBodyNode(&(*ast));
                *ast = CreateCodeNode(*ast); 
                *ast = CreateAssignemtNode(*ast);
                *ast = CreateIdNode(*ast, saveToken->data);
                saveToken = NULL;
            }
            // used to join expression with assignment
            else if(lastNonTerminal == NON_T_EXPR)
            {
                *ast = findDeepestAssignmentNode(&(*ast));
                (*ast)->right = *expr_root;
                saveToken = NULL;
            }
            // concat because of functions like ifj.write, because those are 3 tokens
            else if(token->type == TOKEN_DOT)
            {
                strcat(saveToken->data,token->data);
            }
            else if(token->type == TOKEN_VARIABLE)  
            {
                strcat(saveToken->data,token->data);
            }
            else if(token->type == TOKEN_UNDERSCORE)
            {
                saveToken->data = "_";
            }
        default:
            break;
    }
}

/**
 * @brief converts token type to data type for AST building
 * 
 * @param type token type
 * @return DataType data type
 */
DataType TokenTypeToDataType(Tokentype type)
{
    switch (type)
    {
        case TOKEN_void:
            return T_VOID;
        case TOKEN_i32:
            return T_I32;
        case TOKEN_f64:
            return T_F64;
        case TOKEN_u8:
            return T_U8;
        case TOKEN_i32_NULL:
            return T_I32_N;
        case TOKEN_f64_NULL:
            return T_F64_N;
        case TOKEN_u8_NULL:
            return T_U8_N;
        default:
            return T_DEFAULT;
    }
}

/**
 * @brief checks if current token is interesting meaning ast is going to be build based on that type
 * @param type type of current token
 * @return 1 if token is interesting, 0 if not
 */
int InterestingTokens(Tokentype type)
{
    switch (type)
    {
        case TOKEN_fn:
            return 1;
        case TOKEN_const:
            return 1;
        case TOKEN_var:
            return 1;
        case TOKEN_return:
            return 1;
        case TOKEN_if:
            return 1;
        case TOKEN_while:
            return 1;
        case TOKEN_else:
            return 1;
        case TOKEN_CURLY_RIGHT_PAR:
            return 1;
        case TOKEN_UNDERSCORE:
            return 1;
        case TOKEN_VARIABLE:
            // im already hanfling variables in other cases in buildAST
            // variables are interesting only if they are assignes/function calls
            return lastNonTerminal == NON_T_ID_HELPER ? 1 : 0;
        default:
            return 0;
    }
}