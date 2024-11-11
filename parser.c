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

NonTerminal lastNonTerminal = NON_TERMINAL_UNKOWN;

int Parser(TokenList* list)
{
    Stack* stack = InitStack();
    Error error = 0;
    ASTNode *ast = CreateAST(); 
    ASTNode *root = ast; // potom sa bude posielat do semantiky
    ASTNode *baseCode = CreateCodeNode(ast);
    // ASTNode *funcDec;
    // ASTNode *funcBodyDec;
    Tokentype lastInteresingToken = TOKEN_UNKOWN;
    PushItem(stack, TOKEN_UNKOWN, NON_T_BODY);


    while (!Empty(stack))
    {
        if(list->currToken->type == TOKEN_SPACE || list->currToken->type == TOKEN_COMMENT)
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
                printf("Error found in Parser (LLGrammar)! \n");
                return ERROR_PARSER;
            }

        }
        Top(stack);

        if(Top(stack)->tokenType != list->currToken->type)
        {
            printf("Error found in Parser (Bad top)! \n");
            PrintStack(stack);
            printf(" Top: %s\n Current: %s\n", TokenTypeString(Top(stack)->tokenType), TokenTypeString(list->currToken->type));
            return ERROR_PARSER;
        }
        //PrintStack(stack);
        printf("Popping from stack : %s\n", TokenTypeString(list->currToken->type));
        Pop(stack);

        if(InterestingTokens(list->currToken->type))
        {
            lastInteresingToken = list->currToken->type;
        }

        // build ast
        // mozno by bolo fajn spravit nieco ze by to rekurzivne hladalo kde by sa mal pridat další node
        // napr ak mame zakladal a prisla  by pub ze by sa snazilo najst prazdny code ak by nasle tak by sa pridal funcDec k nemu
        // prisli by paramtere tak by sa pridal parametre a tak dalej
        if (lastInteresingToken != TOKEN_UNKOWN )
        {
            BuildAST(&ast, lastInteresingToken, list->currToken);
            ast = root;
        }

        Token* nextToken = list->currToken->nextToken;
        list->currToken = nextToken;

    }
    printf("Parser finished successfully\n");
    FreeStack(stack);

    return 0;
}

int LLGrammar(Stack* stack, Tokentype type)
{
    NonTerminal nonterminal = Top(stack)->nonTerminal;
    Error error = 0;

    while (Top(stack)->tokenType == TOKEN_UNKOWN)
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
            case NON_T_ID_CONTINUE:
                error = NonTerminalIdContinuePush(stack, type);
            break;
            case NON_T_PARAMS_ENTER:
                error = NonTerminalParamsEnterPush(stack, type);
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
            case NON_T_ELSE:
                error = NonTerminalElsePush(stack, type);
            break;
            case NON_T_PARAMS:
                error = NonTerminalParamsPush(stack, type);
            break;
            case NON_T_NEXT_PARAMS:
                error = NonTerminalNextParamsPush(stack, type);
            break;
            case NON_T_EOL:
                error = NonTerminalEolPush(stack, type);
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
    // <body> -> <eol> <body>
    if(type == TOKEN_EOL)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_BODY);
        PushItem(stack, TOKEN_UNKOWN, NON_T_EOL);
    }
    // <body> -> CONST ID = @IMPORT(STRING); <after_body>
    else if(type == TOKEN_const)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_AFTER_BODY);
        PushItem(stack, TOKEN_SEMICOLON, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_STRING, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_PROLOG, NON_TERMINAL_UNKOWN);  // -------- PRIDAJ TOKEN IMPORT 
        PushItem(stack, TOKEN_ASSIGN, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_VARIABLE, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_const, NON_TERMINAL_UNKOWN);
    }
    //<body> := EOF
    else if (type == TOKEN_EOF)
    {
        PushItem(stack, TOKEN_EOF, NON_TERMINAL_UNKOWN);
    }
    else
    {
        printf("Error in NonTerminalBodyPush! \n");
        return ERROR_PARSER;
    }
    return 0;
}

int NonTerminalAfterBodyPush(Stack* stack, Tokentype type)
{
    // <after_body> -> <eol> <after_body>
    if(type == TOKEN_EOL)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_AFTER_BODY);
        PushItem(stack, TOKEN_UNKOWN, NON_T_EOL);
    }
    // <after_body> -> PUB FN ID (<params>) <return_type> <eol> { <function_body> } <after_body>
    else if(type == TOKEN_pub)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_AFTER_BODY);
        PushItem(stack, TOKEN_CURLY_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_CURLY_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKOWN, NON_T_EOL);
        PushItem(stack, TOKEN_UNKOWN, NON_T_RETURN_TYPE);
        PushItem(stack, TOKEN_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKOWN, NON_T_PARAMS);
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
        printf("Error in NonTerminalAfterBodyPush! \n");
        return ERROR_PARSER;
    }
    return 0;
}

int NonTerminalFunctionBodyPush(Stack* stack, Tokentype type)
{
    // <function_body> -> <var_or_const> ID : <type> = expression; <function_body>
    if(type == TOKEN_const || type == TOKEN_var)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_SEMICOLON, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_PLUS, NON_TERMINAL_UNKOWN); // EXPRESION NESKOR zatial tam dam token_plus
        PushItem(stack, TOKEN_ASSIGN, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKOWN, NON_T_TYPE);
        PushItem(stack, TOKEN_COLON,NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_VARIABLE, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKOWN, NON_T_VAR_OR_CONST);
    }
    //<function_body> -> ID <id_continue>;  <function_body>
    else if(type == TOKEN_VARIABLE)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_SEMICOLON, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKOWN, NON_T_ID_CONTINUE);
        PushItem(stack, TOKEN_VARIABLE, NON_TERMINAL_UNKOWN);
    }
    //<function_body> -> IF (expression) <bars> <eol> {<function_body>} <else> <function_body>
    else if(type == TOKEN_if)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_UNKOWN, NON_T_ELSE);
        PushItem(stack, TOKEN_CURLY_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_CURLY_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKOWN, NON_T_EOL);
        PushItem(stack, TOKEN_UNKOWN, NON_T_BARS);
        PushItem(stack, TOKEN_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_PLUS, NON_TERMINAL_UNKOWN); // EXPRESION NESKOR zatial tam dam token_plus
        PushItem(stack, TOKEN_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_if, NON_TERMINAL_UNKOWN);
    }
    //<function_body> -> WHILE (expression) <bars> <eol> {<function_body>} <function_body>
    else if(type == TOKEN_while)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_CURLY_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_CURLY_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKOWN, NON_T_EOL);
        PushItem(stack, TOKEN_UNKOWN, NON_T_BARS);
        PushItem(stack, TOKEN_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_PLUS, NON_TERMINAL_UNKOWN); // EXPRESION NESKOR zatial tam dam token_plus
        PushItem(stack, TOKEN_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_while, NON_TERMINAL_UNKOWN);
    }
    //<function_body> -> RETURN expression; <function_body>
    else if(type == TOKEN_return)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_SEMICOLON, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_PLUS, NON_TERMINAL_UNKOWN); // EXPRESION NESKOR zatial tam dam token_plus
        PushItem(stack, TOKEN_return, NON_TERMINAL_UNKOWN);
    }
    //<function_body> -> <eol> <function_body>
    else if(type == TOKEN_EOL)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_UNKOWN, NON_T_EOL);
    }
    //<function_body> -> ε
    return 0;

}

int NonTerminalIdContinuePush(Stack* stack, Tokentype type)
{
    // <id_continue> -> = expression
    if(type == TOKEN_ASSIGN)
    {
        PushItem(stack, TOKEN_PLUS, NON_TERMINAL_UNKOWN); // EXPRESION NESKOR zatial tam dam token_plus
        PushItem(stack, TOKEN_ASSIGN, NON_TERMINAL_UNKOWN);
    }
    // <id_continue> -> .ID(<params_enter>)    // BUILD IN FUNCTIONS IG IDK
    else if(type == TOKEN_DOT)
    {
        PushItem(stack, TOKEN_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKOWN, NON_T_PARAMS_ENTER);
        PushItem(stack, TOKEN_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_VARIABLE, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_DOT, NON_TERMINAL_UNKOWN);
    }
    // <id_continue> -> (<params_enter>)
    else if(type == TOKEN_LEFT_PAR)
    {
        PushItem(stack, TOKEN_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKOWN, NON_T_PARAMS_ENTER);
        PushItem(stack, TOKEN_LEFT_PAR, NON_TERMINAL_UNKOWN);
    }
    else
    {
        printf("Error in NonTerminalIdContinuePush! \n");
        return ERROR_PARSER;
    }
    return 0;
}

int NonTerminalParamsEnterPush(Stack* stack, Tokentype type)
{
    // <params_enter> -> <term> <next_params_enter>
    if(type == TOKEN_VARIABLE || type == TOKEN_f64 || type == TOKEN_i32 || type == TOKEN_u8)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_NEXT_PARAMS_ENTER);
        PushItem(stack, TOKEN_UNKOWN, NON_T_TERM);
    }
    else
    {
        printf("Error in NonTerminalParamsEnterPush! \n");
        return ERROR_PARSER;
    }
    return 0;
}

int NonTerminalNextParamsEnterPush(Stack* stack, Tokentype type)
{
    // <next_params_enter> -> , <params_enter>
    if(type == TOKEN_COMMA)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_PARAMS_ENTER);
        PushItem(stack, TOKEN_COMMA, NON_TERMINAL_UNKOWN);
    }
    else 
    {
        printf("Error in NonTerminalNextParamsEnterPush! \n");
        return ERROR_PARSER;
    }
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
        printf("Error in NonTerminalVarOrConstPush! \n");
        return ERROR_PARSER;
    }
    return 0;
}

int NonTerminalEolPush(Stack* stack, Tokentype type)
{
    // <eol> := EOL <eol>
    if(type == TOKEN_EOL)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_EOL);
        PushItem(stack, TOKEN_EOL, NON_TERMINAL_UNKOWN);
    }
    // <eol> := ε // HADAM ZE TAKTO IG IDK

    return 0;
}

int NonTerminalParamsPush(Stack* stack, Tokentype type)
{
    // <params> -> ID : <type> <next_params>
    if(type == TOKEN_VARIABLE)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_NEXT_PARAMS);
        PushItem(stack, TOKEN_UNKOWN, NON_T_TYPE);
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
        PushItem(stack, TOKEN_UNKOWN, NON_T_PARAMS);
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

int NonTerminalElsePush(Stack* stack, Tokentype type)
{
    // <else> -> ELSE {<function_body>}
    if(type == TOKEN_else)
    {
        PushItem(stack, TOKEN_CURLY_RIGHT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_UNKOWN, NON_T_FUNCTION_BODY);
        PushItem(stack, TOKEN_CURLY_LEFT_PAR, NON_TERMINAL_UNKOWN);
        PushItem(stack, TOKEN_else, NON_TERMINAL_UNKOWN);
    }
    // <else> -> ε

    return 0;
}

int NonTerminalReturnTypePush(Stack* stack, Tokentype type)
{
    // <return_type> -> <type>
    if(type == TOKEN_i32 || type == TOKEN_f64 || type == TOKEN_u8)
    {
        PushItem(stack, TOKEN_UNKOWN, NON_T_TYPE);
    }
    // <return_type> -> VOID
    else if(type == TOKEN_void)
    {
        PushItem(stack, TOKEN_void, NON_TERMINAL_UNKOWN);
    }
    else
    {
        printf("Error in NonTerminalReturnTypePush! \n");
        PrintStack(stack);
        printf("Got : %s \n", TokenTypeString(type));
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
    else
    {
        printf("Error in NonTerminalTypePush! \n Got : %s \n Needed : %s \n", TokenTypeString(type), NonTerminalToString(stack->items[stack->top]->nonTerminal));
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
    else if(type == TOKEN_f64)
    {
        PushItem(stack, TOKEN_f64, NON_TERMINAL_UNKOWN);
    }
    // <term> -> VALUE_INT
    else if(type == TOKEN_i32)
    {
        PushItem(stack, TOKEN_i32, NON_TERMINAL_UNKOWN);
    }
    // <term> -> VALUE_STRING
    else if(type == TOKEN_u8)
    {
        PushItem(stack, TOKEN_u8, NON_TERMINAL_UNKOWN);
    }
    else
    {
        printf("Error in NonTerminalTermPush! \n");
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
        if ((*ast)->left == NULL) return *ast;
        return findDeepestCodeNode(&(*ast)->left);
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

void BuildAST(ASTNode** ast, Tokentype interestingToken, Token* token)
{
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
            break;
        case TOKEN_return:
             if(token->type == TOKEN_return)
            {
                *ast = findDeepestFunctionBodyNode(&(*ast));
                *ast = CreateCodeNode(*ast);
                *ast = CreateReturnNode(*ast);
            }
            break;
        case TOKEN_if:
            *ast = CreateIfNode(*ast);
            break;
        case TOKEN_while:
            *ast = CreateWhileNode(*ast);
            break;
        case TOKEN_else:
            *ast = CreateElseNode(*ast);
            break;
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
        // case TOKEN_if:
        //     return 1;
        // case TOKEN_while:
        //     return 1;
        // case TOKEN_else:
        //     return 1;
        // case TOKEN_SEMICOLON:
        //     return 1;
        break;
        default:
            return 0;
    }
}