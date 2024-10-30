#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "stack.h"
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
    ASTNode *funcDec;
    ASTNode *funcBodyDec;
    Tokentype lastInteresingToken = TOKEN_UNKOWN;
    PushItem(stack, TOKEN_UNKOWN, NON_T_BODY);

    StackItem* item = InitStackItem();
    StackItem* item_popped = InitStackItem();

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
        item = Top(stack);

        if(Top(stack)->tokenType != list->currToken->type)
        {
            printf("Error found in Parser (Bad top)! \n");
            PrintStack(stack);
            printf(" Top: %s\n Current: %s\n", TokenTypeString(Top(stack)->tokenType), TokenTypeString(list->currToken->type));
            return ERROR_PARSER;
        }
        //PrintStack(stack);
        printf("Popping from stack : %s\n", TokenTypeString(list->currToken->type));
        item_popped = Pop(stack);

        //SEMANTICS
        if(lastNonTerminal == NON_T_AFTER_BODY && list->currToken->type == TOKEN_pub)
        {
            funcDec = CreateCodeNode(ast);
        }
        else if(lastNonTerminal == NON_T_AFTER_BODY && list->currToken->type == TOKEN_fn)
        {
            funcDec = CreateFunDeclNode(funcDec);
            // last intersting token
            lastInteresingToken = list->currToken->type; 
            funcBodyDec = CreateCodeNode(funcDec); // hadam ze toto vytvory function body na pravo od fundec
        }
        else if(lastInteresingToken == TOKEN_fn && list->currToken->type == TOKEN_VARIABLE)
        {
            //printf("SME TUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUu");
            funcDec = CreateIdNode(funcDec, list->currToken->data); 
            lastInteresingToken = TOKEN_RIGHT_PAR;
        }
        else if(lastNonTerminal == NON_T_PARAMS && list->currToken->type == TOKEN_VARIABLE)
        {
            funcDec = CreateParamNode(funcDec, list->currToken->data); // neviem ako pridat typ id A
        }
        else if(lastNonTerminal == NON_T_NEXT_PARAMS && list->currToken->type == TOKEN_VARIABLE)
        {
            funcDec = CreateParamNode(funcDec, list->currToken->data); // neviem ako pridat typ id B
        }

        DisplayAST(ast);
        //SEMANTICS

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
        Pop(stack); // pop pre neterminal na vrchu stacku 
        lastNonTerminal = nonterminal;
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
        PushItem(stack, TOKEN_DOT, NON_TERMINAL_UNKOWN);  // -------- PRIDAJ TOKEN IMPORT 
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
        PushItem(stack, TOKEN_EQUAL, NON_TERMINAL_UNKOWN);
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
    if(type == TOKEN_EQUAL)
    {
        PushItem(stack, TOKEN_PLUS, NON_TERMINAL_UNKOWN); // EXPRESION NESKOR zatial tam dam token_plus
        PushItem(stack, TOKEN_EQUAL, NON_TERMINAL_UNKOWN);
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

