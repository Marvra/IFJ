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

int Parser(ASTNode** tree, TokenList* list)
{
    int errorExpr = 0;
    Token *saveToken = InitToken();
    Stack* stack = InitStack();
    Error error = 0;
    ASTNode *ast = CreateAST(); 
    ASTNode *root = ast; // potom sa bude posielat do semantiky
    ASTNode *baseCode = CreateCodeNode(ast);
    ASTNode *expr_root = NULL;
    Tokentype lastInteresingToken = TOKEN_UNKNOWN;
    PushItem(stack, TOKEN_UNKNOWN, NON_T_BODY);


    while (!Empty(stack))
    {
        if(list->currToken->type == TOKEN_SPACE || list->currToken->type == TOKEN_COMMENT || list->currToken->type == TOKEN_EOL)
        {
            Token* nextToken = list->currToken->nextToken;
            list->currToken = nextToken;
            continue;
        }

        if(Top(stack)->nonTerminal != NON_TERMINAL_UNKOWN)
        {
            error = LLGrammar(stack, list->currToken->type);

            if(error != 0)
            {
                //printf("Error found in Parser (LLGrammar)! \n");
                return ERROR_PARSER;
            }

        }
        Top(stack);

        if(Top(stack)->nonTerminal == NON_T_EXPR) {
            Pop(stack);
            lastNonTerminal = NON_T_EXPR;
            errorExpr = expr_start(&expr_root, &list, Top(stack)->tokenType);
            if (errorExpr != 0) {
                //printf("Error found in EXPRESION (expr_start)! \n");
                return ERROR_PARSER;
            }
        }

        if(Top(stack)->tokenType != list->currToken->type)
        {
            //printf("Error found in Parser (Bad top)! \n");
            //PrintStack(stack);
            //printf(" Top: %s\n Current: %s\n", TokenTypeString(Top(stack)->tokenType), TokenTypeString(list->currToken->type));
            return ERROR_PARSER;
        }
        //PrintStack(stack);
        //printf("Popping from stack : %s\n", TokenTypeString(list->currToken->type));
        Pop(stack);

        if(InterestingTokens(list->currToken->type))
        {
            lastInteresingToken = list->currToken->type;
        }

        // NEMENIT NEJAKO FUNGUJE<3
        if (lastInteresingToken != TOKEN_UNKNOWN )
        {
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

        expr_root = NULL; // vycissti lebo potom dalsi expresion by sa ti zapisoval do toho isteho
        // NEMENIT NEJAKO FUNGUJE<3

        Token* nextToken = list->currToken->nextToken;
        list->currToken = nextToken;

    }
    //printf("Parser finished successfully\n");
    FreeStack(stack);

    //exportASTToDot(root);
    *tree = ast;

    return 0;
}

int LLGrammar(Stack* stack, Tokentype type)
{
    NonTerminal nonterminal = Top(stack)->nonTerminal;
    Error error = 0;

    while (Top(stack)->tokenType == TOKEN_UNKNOWN && Top(stack)->nonTerminal != NON_T_EXPR)
    {
        lastNonTerminal = nonterminal;
        Pop(stack); // pop pre neterminal na vrchu stacku 
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
        //printf("Error in NonTerminalBodyPush! \n");
        return ERROR_PARSER;
    }
    return 0;
}

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
        //printf("Error in NonTerminalAfterBodyPush! \n");
        return ERROR_PARSER;
    }
    return 0;
}

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
        PushItem(stack, TOKEN_UNKNOWN, NON_T_EXPR); // EXPRESION NESKOR zatial tam dam token_plus
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
        PushItem(stack, TOKEN_UNKNOWN, NON_T_EXPR); // EXPRESION NESKOR zatial tam dam token_plus
        PushItem(stack, TOKEN_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_while, NON_TERMINAL_UNKOWN);
    }
    //<function_body> -> RETURN expression; <function_body>
    else if(type == TOKEN_return)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_SEMICOLON, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_EXPR); // EXPRESION NESKOR zatial tam dam token_plus
        PushItem(stack, TOKEN_return, NON_TERMINAL_UNKOWN);
    }
    //<function_body> -> ε
    return 0;

}

int NonTerminalDeclarationContinuePush(Stack* stack, Tokentype type)
{
    // <declaration_continue> -> : <type> = expression
    if(type == TOKEN_COLON)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_EXPR); // EXPRESION NESKOR zatial tam dam token_plus
        PushItem(stack, TOKEN_ASSIGN, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_TYPE);
        PushItem(stack, TOKEN_COLON, NON_TERMINAL_UNKOWN);
    }
    // <declaration_continue> -> = expression
    else if(type == TOKEN_ASSIGN)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_EXPR); // EXPRESION NESKOR zatial tam dam token_plus
        PushItem(stack, TOKEN_ASSIGN, NON_TERMINAL_UNKOWN);
    }
    else
    {
        //printf("Error in NonTerminalDeclarationContinuePush! \n");
        return ERROR_PARSER;
    }
    return 0;
}

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
        PushItem(stack, TOKEN_UNKNOWN, NON_T_EXPR); // EXPRESION NESKOR zatial tam dam token_plus
        PushItem(stack, TOKEN_ASSIGN, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNDERSCORE, NON_TERMINAL_UNKOWN);
    }
    else
    {
        //printf("Error in NonTerminalIdHelper! \n");
        return ERROR_PARSER;
    }
    return 0;
}

int NonTerminalIdContinuePush(Stack* stack, Tokentype type)
{
    // <id_continue> -> = expression
    if(type == TOKEN_ASSIGN)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_EXPR); // EXPRESION NESKOR zatial tam dam token_plus
        PushItem(stack, TOKEN_ASSIGN, NON_TERMINAL_UNKOWN);
    }
    // <id_continue> -> .ID(<params_enter>)    // BUILD IN FUNCTIONS IG IDK
    else if(type == TOKEN_DOT)
    {
        PushItem(stack, TOKEN_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_PARAMS_ENTER);
        PushItem(stack, TOKEN_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_VARIABLE, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_DOT, NON_TERMINAL_UNKOWN);
    }
    // <id_continue> -> (<params_enter>)
    else if(type == TOKEN_LEFT_PAR)
    {
        PushItem(stack, TOKEN_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_PARAMS_ENTER);
        PushItem(stack, TOKEN_LEFT_PAR, NON_TERMINAL_UNKOWN);
    }
    else
    {
        //printf("Error in NonTerminalIdContinuePush! \n");
        return ERROR_PARSER;
    }
    return 0;
}

int NonTerminalParamsEnterPush(Stack* stack, Tokentype type)
{
    // <params_enter> -> <term> <next_params_enter>
    if(type == TOKEN_INTEGER || type == TOKEN_FLOAT || type == TOKEN_STRING || type == TOKEN_VARIABLE)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_NEXT_PARAMS_ENTER);
        PushItem(stack, TOKEN_UNKNOWN, NON_T_TERM);
    }
    // <params_enter> -> ε
    return 0;
}

int NonTerminalNextParamsEnterPush(Stack* stack, Tokentype type)
{
    // <next_params_enter> -> , <params_enter>
    if(type == TOKEN_COMMA)
    {
        PushItem(stack, TOKEN_UNKNOWN, NON_T_PARAMS_ENTER);
        PushItem(stack, TOKEN_COMMA, NON_TERMINAL_UNKOWN);
    }
    // <next_params_enter> -> ε
    return 0;
}

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
        //printf("Error in NonTerminalVarOrConstPush! \n");
        return ERROR_PARSER;
    }
    return 0;
}

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

int NonTerminalReturnTypePush(Stack* stack, Tokentype type)
{
    // <return_type> -> <type>
    if(type == TOKEN_i32 || type == TOKEN_f64 || type == TOKEN_u8)
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
        //printf("Error in NonTerminalReturnTypePush! \n");
        //PrintStack(stack);
        //printf("Got : %s \n", TokenTypeString(type));
        return ERROR_PARSER;
    }
    return 0;
}

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
        //printf("Error in NonTerminalTypePush! \n Got : %s \n Needed : %s \n", TokenTypeString(type), NonTerminalToString(stack->items[stack->top]->nonTerminal));
        return ERROR_PARSER;
    }
    return 0;
}

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
    else
    {
        //printf("Error in NonTerminalTermPush! \n");
        return ERROR_PARSER;
    }
    return 0;
}

ASTNode* findDeepestCodeNode(ASTNode** ast) 
{
    if ((*ast)->left == NULL) {
        return *ast;
    } else {
        return findDeepestCodeNode(&(*ast)->left);
    }
}
// finds deepest code node and returns FunDec node
ASTNode* findDeepestFunDecNode(ASTNode** ast)
{
    *ast = findDeepestCodeNode(&(*ast));
    return (*ast)->right  == NULL ? *ast : (*ast)->right;
}


// MOZNO NEPOTREBUJEME BO CREATEPARAMNODE PREJDE UZ NA SPODOK PARAMS DANEJ FUNKCIE
// ALE ASI POTREBUJEME KVOLI ZAPISU TYPE 
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
// MOZNO NEPOTREBUJEME BO CREATEPARAMNODE PREJDE UZ NA SPODOK PARAMS DANEJ FUNKCIE

ASTNode* findDeepestFuncCodeNode(ASTNode** ast)
{
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

ASTNode* findDeepestIfElseNodeHelp(ASTNode** ast)
{
    if ((*ast)->left == NULL && (*ast)->right->type == TYPE_WHILE)
    {
        temp = (*ast)->right; // saving while
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

ASTNode* findDeepestWhileNodeHelp(ASTNode** ast) {
    if ((*ast)->left == NULL && (*ast)->right->type == TYPE_WHILE)
    {
        temp = (*ast)->right; // saving while
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


void BuildAST(ASTNode** expr_root, ASTNode** ast, Tokentype interestingToken, Token* token, Token* saveToken)
{
    // if(lastNonTerminal == NON_T_ID_HELPER || lastNonTerminal == NON_T_ID_CONTINUE || lastNonTerminal == NON_T_PARAMS_ENTER || lastNonTerminal == NON_T_NEXT_PARAMS_ENTER)
    // {
    //     if(lastNonTerminal == NON_T_ID_HELPER)
    //     {
    //         *ast = findDeepestFunDecNode(&(*ast));
    //     }
    //     else if(lastNonTerminal == NON_T_ID_CONTINUE)
    //     {
    //         if (token->type == TOKEN_ASSIGN)
    //         {
    //             *ast = findDeepestParamNode(&(*ast));
    //             (*ast)->right = *expr_root;
    //         }
    //         else if (token->type == TOKEN_DOT)
    //         {
    //             *ast = findDeepestParamNode(&(*ast));
    //             (*ast)->right = *expr_root;
    //         }
            
    //     }
    // }
    switch (interestingToken)
    {
        case TOKEN_fn:
            if(token->type == TOKEN_fn)
            {
                *ast = findDeepestCodeNode(&(*ast));
                if((*ast)->right != NULL)   // creates new code node under the top one (bo musime zapisat func do dalsej code node)
                {
                    *ast = CreateCodeNode(*ast);
                }
                *ast = CreateFunDeclNode(*ast);
            }
            else if(token->type == TOKEN_VARIABLE)
            {
                // function id 
                *ast = findDeepestFunDecNode(&(*ast));
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
            // param type
            else if(token->type == TOKEN_void || token->type == TOKEN_i32 || token->type == TOKEN_f64 || token->type == TOKEN_u8)
            {
                *ast = findDeepestFunDecNode(&(*ast));
                *ast = CreateTypeNode(*ast, TokenTypeToDataType(token->type));
            }
        break;
        case TOKEN_const:
            if(lastNonTerminal == NON_T_BODY){
                break; // dont check const for prolog 
            }
            if(token->type == TOKEN_const)
            {
                *ast = findDeepestFunctionBodyNode(&(*ast));
                *ast = CreateCodeNode(*ast);
                *ast = CreateConDeclNode(*ast);
            }
            else if(token->type == TOKEN_VARIABLE)
            {
                // function id 
                *ast = findDeepestConstNode(&(*ast));
                *ast = CreateIdNode(*ast, token->data);
            }
            else if(token->type == TOKEN_i32 || token->type == TOKEN_f64 || token->type == TOKEN_u8)
            {
                *ast = findDeepestConstNode(&(*ast));
                *ast = CreateTypeNode(*ast, TokenTypeToDataType(token->type));
            }
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
            else if(token->type == TOKEN_VARIABLE)
            {
                // function id 
                *ast = findDeepestVarNode(&(*ast));
                *ast = CreateIdNode(*ast, token->data);
            }
            else if(token->type == TOKEN_i32 || token->type == TOKEN_f64 || token->type == TOKEN_u8)
            {
                *ast = findDeepestVarNode(&(*ast));
                *ast = CreateTypeNode(*ast, TokenTypeToDataType(token->type));
            }
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
            else if(lastNonTerminal == NON_T_EXPR)
            {
                *ast = findDeepestReturnNode(&(*ast));
                (*ast)->right = *expr_root;
            }
            break;
        case TOKEN_if:
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
            *ast = findFirstClosedIfNode(&(*ast));
            if(*ast != NULL)
            {
                *ast = CreateElseNode(*ast);
            }
            break;
        case TOKEN_CURLY_RIGHT_PAR:
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
            if(lastNonTerminal == NON_T_ID_HELPER)
            {
                saveToken->data = strdup(token->data);
                saveToken->type = token->type;
                saveToken->dataLength = token->dataLength;
            }
            else if(token->type == TOKEN_LEFT_PAR) // DOROB FKN ARGUMENTY DPC
            {
                *ast = findDeepestFunctionBodyNode(&(*ast));
                *ast = CreateCodeNode(*ast); 
                *ast = CreateFunCallNode(*ast);
                *ast = CreateIdNode(*ast, saveToken->data);
                saveToken = NULL;
            }
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
                
                saveToken = NULL;
            }
            else if(token->type == TOKEN_ASSIGN)
            {
                *ast = findDeepestFunctionBodyNode(&(*ast));
                *ast = CreateCodeNode(*ast); 
                *ast = CreateAssignemtNode(*ast);// create assignment node
                *ast = CreateIdNode(*ast, saveToken->data);
                saveToken = NULL;
            }
            else if(lastNonTerminal == NON_T_EXPR)
            {
                *ast = findDeepestAssignmentNode(&(*ast));
                (*ast)->right = *expr_root;
                saveToken = NULL;
            }
            // TOTO KVOLI ifj.write() funkciam etc
            else if(token->type == TOKEN_DOT)
            {
                strcat(saveToken->data,token->data);
            }
            else if(token->type == TOKEN_VARIABLE)  
            {
                strcat(saveToken->data,token->data);
            }
        default:
            break;
    }
}


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
        default:
            return T_DEFAULT;
    }
}


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
        break;
        case TOKEN_VARIABLE:
            return lastNonTerminal == NON_T_ID_HELPER ? 1 : 0;
        default:
            return 0;
    }
}

// // Function to print a single node and its children in DOT format
// void printDotAST(ASTNode* node, FILE* file) {
//     if (node == NULL) return;

//     // Print the current node's label, assuming `type` or similar gives a name for the node
//     f//printf(file, "    \"%p\" [label=\"%s\"];\n", (void*)node, NodeTypeToString(node->type));

//     // Print edges for left and right children if they exist
//     if (node->left) {
//         f//printf(file, "    \"%p\" -> \"%p\";\n", (void*)node, (void*)node->left);
//         printDotAST(node->left, file);
//     }
//     if (node->right) {
//         f//printf(file, "    \"%p\" -> \"%p\";\n", (void*)node, (void*)node->right);
//         printDotAST(node->right, file);
//     }
// }

// // Main function to export AST to a DOT file
// void exportASTToDot(ASTNode* root) {
//     FILE* file = fopen("ast.txt", "w");
//     if (file == NULL) {
//         perror("Failed to open file for DOT output");
//         return;
//     }
//     f//printf(file, "digraph AST {\n");
//     printDotAST(root, file);
//     f//printf(file, "}\n");
//     fclose(file);
// }