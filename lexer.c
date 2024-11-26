#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "token.h"
#include "lexer.h"
#include "error.h"

int Lexer(FILE* file, TokenList* list)
{
    Token* token = InitToken();
    Tokentype currType = TOKEN_UNKNOWN;
    State errorState = STATE_OK;
    list->firstToken = token;

    while (currType != TOKEN_EOF)
    {
        list->currToken = token;
        errorState = FSM(file, token);
        
        if(errorState == STATE_ERROR)
        {
            return ERROR_LEXER;
        }

        currType = token->type;

        GetNextToken(&token);
    }
    //PrintTokenList(list);
    return 0;
    
}

State FSM(FILE* file, Token* token)
{
    char c;
    State state = STATE_START;
    State errorState = STATE_OK;

    while(token->type == TOKEN_UNKNOWN)
    {
        c = getc(file);

        switch (state)
        {
            case STATE_START:
                state = GetFirstState(c);
            break;

            case STATE_UNDERSCORE:
                if(isalnum(c) || c == '_') state = STATE_VARIABLE;
                else token->type = TOKEN_UNDERSCORE;
            break;

            case STATE_VARIABLE:
                if(isalnum(c) || c == '_') state = STATE_VARIABLE;
                else token->type = TOKEN_VARIABLE;
            break;

            case STATE_ZERO:
                if(c == '.') state = STATE_FLOAT_DOT;
                else if(c == 'e' || c == 'E') state = STATE_FLOAT_EXP;
                else if(isdigit(c)) errorState = STATE_ERROR;
                else token->type = TOKEN_INTEGER;
            break;


            // INTEGER START
            case STATE_INTEGER:
                if(isdigit(c)) state = STATE_INTEGER;
                else if(c == '.') state = STATE_FLOAT_DOT;
                else if(c == 'e' || c == 'E') state = STATE_FLOAT_EXP;
                else token->type = TOKEN_INTEGER;
            break;
            // INTEGER END

            // FLOAT START
            case STATE_FLOAT_DOT:
            case STATE_FLOAT_DIGIT:
            case STATE_FLOAT_EXP:
            case STATE_FLOAT_EXP_PM:
            case STATE_FLOAT_END:
                errorState = GetFloatTokenType(c, &state, token);
            break;
            // FLOAT END
            
            // STRING START
            case STATE_STRING_START:
            case STATE_STRING_SLASH:
            case STATE_STRING_HEX1:
            case STATE_STRING_HEX2:
            case STATE_STRING_END:
                errorState = GetStringTokenType(c, &state, token);
            break;
            // STRING END

            case STATE_PROLOG:
            case STATE_PROLOG_i:
            case STATE_PROLOG_m:
            case STATE_PROLOG_p:
            case STATE_PROLOG_o:
            case STATE_PROLOG_r:
            case STATE_PROLOG_t:
                errorState = GetPrologTokenType(c, &state, token);
            break;


            // NULL TYPE START
            case STATE_NULL_START:
            case STATE_NULL_RBRACKET:
            case STATE_NULL_TYPE:
                errorState = GetNullTypesTokenType(c, &state, token);
            break;
            // NULL TYPE END

            // u8 check start
            case STATE_SQUARE_LEFT_PAR:
                if(c == ']') state = STATE_SQUARE_RIGHT_PAR;
                else errorState = STATE_ERROR;
            break;

            case STATE_SQUARE_RIGHT_PAR:
                if(isalnum(c)) state = STATE_VARIABLE;
                else errorState = STATE_ERROR;
            break;
            // u8 check end

            case STATE_PLUS:
                token->type = TOKEN_PLUS;
            break;
            case STATE_MINUS:
                token->type = TOKEN_MINUS;
            break;
            case STATE_MUL:
                token->type = TOKEN_MUL;
            break;
            case STATE_DIV:
                if(c == '/') state = STATE_COMMENT;
                else token->type = TOKEN_DIV;
            break;
            case STATE_COMMENT:
                if(c == '\n' || c==EOF) token->type = TOKEN_COMMENT;
            break;
            case STATE_ASSIGN:
                if(c == '=') state = STATE_EQUAL;
                else token->type = TOKEN_ASSIGN;
            break;
            case STATE_EQUAL:
                token->type = TOKEN_EQUAL;
            break;
            case STATE_LESSER:
                if(c == '=') state = STATE_LESSER_EQUAL;
                else token->type = TOKEN_LESSER;
            break;
            case STATE_LESSER_EQUAL:
                token->type = TOKEN_LESSER_EQUAL;
            break;
            case STATE_GREATER:
                if(c == '=') state = STATE_GREATER_EQUAL;
                else token->type = TOKEN_GREATER;
            break;
            case STATE_GREATER_EQUAL:
                token->type = TOKEN_GREATER_EQUAL;
            break;
            case STATE_NOT:
                if(c == '=') state = STATE_NOT_EQUAL;
                else return STATE_ERROR;
            break;
            case STATE_NOT_EQUAL:
                token->type = TOKEN_NOT_EQUAL;
            break;
            case STATE_LEFT_PAR:
                token->type = TOKEN_LEFT_PAR;
            break;
            case STATE_RIGHT_PAR:
                token->type = TOKEN_RIGHT_PAR;
            break;
            case STATE_CURLY_LEFT_PAR:
                token->type = TOKEN_CURLY_LEFT_PAR;
            break;
            case STATE_CURLY_RIGHT_PAR:
                token->type = TOKEN_CURLY_RIGHT_PAR;
            break;
            case STATE_SEMICOLON:
                token->type = TOKEN_SEMICOLON;
            break;
            case STATE_COLON:
                token->type = TOKEN_COLON;
            break;
            case STATE_BAR:
                token->type = TOKEN_BAR;
            break;
            case STATE_COMMA:
                token->type = TOKEN_COMMA;
            break;
            case STATE_DOT:
                token->type = TOKEN_DOT;
            break;
            case STATE_EOF:
                token->type = TOKEN_EOF;
            break;
            case STATE_EOL:
                token->type = TOKEN_EOL;
            break;
            case STATE_SPACE:
                token->type = TOKEN_SPACE;
            break;
            default:
                errorState = STATE_ERROR;
            break;
        }

        if(errorState == STATE_ERROR)
        {
            return errorState;
        }

        if(token->type != TOKEN_UNKNOWN) 
        {
            if(token->type == TOKEN_VARIABLE)
            {
                CheckKeyword(token);
            }
            else if(token->type == TOKEN_NULL_TYPE)
            {
                ungetc(c,file);
                return CheckForNullType(token);
            }

            ungetc(c,file);
            return errorState;
        }

        DataToToken(c, token);
    }
    return errorState;
}
State GetFirstState(char c)
{
    if(isalpha(c)) return STATE_VARIABLE;
    else if(c=='_') return STATE_UNDERSCORE;
    else if(isdigit(c) && c != '0') return STATE_INTEGER;
    else if(c=='0') return STATE_ZERO;
    else if(c=='"') return STATE_STRING_START;
    else if(c=='?') return STATE_NULL_START;
    else if(c=='+') return STATE_PLUS;
    else if(c=='-') return STATE_MINUS;
    else if(c=='*') return STATE_MUL;
    else if(c=='/') return STATE_DIV;
    else if(c=='=') return STATE_ASSIGN;
    else if(c=='<') return STATE_LESSER;
    else if(c=='>') return STATE_GREATER;
    else if(c=='!') return STATE_NOT;
    else if(c=='(') return STATE_LEFT_PAR;
    else if(c==')') return STATE_RIGHT_PAR;
    else if(c=='{') return STATE_CURLY_LEFT_PAR;
    else if(c=='}') return STATE_CURLY_RIGHT_PAR;
    else if(c=='[') return STATE_SQUARE_LEFT_PAR;
    else if(c==';') return STATE_SEMICOLON;
    else if(c==':') return STATE_COLON;
    else if(c=='|') return STATE_BAR;
    else if(c==',') return STATE_COMMA;
    else if(c=='.') return STATE_DOT;
    else if(c=='@') return STATE_PROLOG;
    else if(c=='\n') return STATE_EOL;
    else if(isspace(c)) return STATE_SPACE;
    else if(c==EOF) return STATE_EOF;
    else return STATE_ERROR;
}

State GetNullTypesTokenType(char c, State* state, Token* token)
{
    switch (*state)
    {
        case STATE_NULL_START:
            if(isalnum(c)) *state = STATE_NULL_TYPE;
            else if(c == '[') *state = STATE_NULL_RBRACKET;
            else return STATE_ERROR;
        break;
        case STATE_NULL_RBRACKET:
            if(c == ']') *state = STATE_NULL_TYPE;
            else return STATE_ERROR;
        break;
        case STATE_NULL_TYPE:
            if(isalnum(c)) *state = STATE_NULL_START;
            else token->type = TOKEN_NULL_TYPE;
        break;
        default:
            return STATE_ERROR;
        break;
    }
    return STATE_OK;
}

State GetFloatTokenType(char c, State* state, Token* token)
{
    switch (*state)
    {
        case STATE_FLOAT_DOT:
            if(isdigit(c)) *state = STATE_FLOAT_DIGIT;
            else return STATE_ERROR;
        break;
        case STATE_FLOAT_DIGIT:
            if(isdigit(c)) *state = STATE_FLOAT_DIGIT;
            else if(c == 'e' || c == 'E') *state = STATE_FLOAT_EXP;
            else token->type = TOKEN_FLOAT;
        break;
        case STATE_FLOAT_EXP:
            if(c == '+' || c == '-') *state = STATE_FLOAT_EXP_PM;
            else if(isdigit(c)) *state = STATE_FLOAT_END;
            else  return STATE_ERROR;
        break;
        case STATE_FLOAT_EXP_PM:
            if(isdigit(c)) *state = STATE_FLOAT_END;
            else  return STATE_ERROR;
        break;
        case STATE_FLOAT_END:
            if(isdigit(c)) *state = STATE_FLOAT_END;
            else  return STATE_ERROR;
        break;
        default:
            return STATE_ERROR;
        break;
    }
    return STATE_OK;
}

State GetStringTokenType(char c, State* state, Token* token)
{
    switch (*state)
    {
        case STATE_STRING_START:
            if(c == '"') *state = STATE_STRING_END;
            else if(c == '\\') *state = STATE_STRING_SLASH;
            else if(c >= 32) *state = STATE_STRING_START;
            else return STATE_ERROR;
        break;
        case STATE_STRING_SLASH:
            if(c == 'n' || c == 'r' || c == 't' || c == '\'' || c == '"' || c == '\\') *state = STATE_STRING_START;
            else if(c == 'x') *state = STATE_STRING_HEX1;
            else return STATE_ERROR;
        break;
        case STATE_STRING_HEX1:
            if(isxdigit(c)) *state = STATE_STRING_HEX2;
            else return STATE_ERROR;
        break;
        case STATE_STRING_HEX2:
            if(isxdigit(c)) *state = STATE_STRING_START;
            else return STATE_ERROR;
        break;
        case STATE_STRING_END:
            token->type = TOKEN_STRING;
        break;
        default:
            return STATE_ERROR;
        break;
    }
    return STATE_OK;
}

State GetPrologTokenType(char c, State* state, Token* token)
{
    switch (*state)
    {
        case STATE_PROLOG:
            if(c == 'i') *state = STATE_PROLOG_i;
            else return STATE_ERROR;
        break;
        case STATE_PROLOG_i:
            if(c == 'm') *state = STATE_PROLOG_m;
            else return STATE_ERROR;
        break;
        case STATE_PROLOG_m:
            if(c == 'p') *state = STATE_PROLOG_p;
            else return STATE_ERROR;
        break;  
        case STATE_PROLOG_p:
            if(c == 'o') *state = STATE_PROLOG_o;
            else return STATE_ERROR;
        break;
        case STATE_PROLOG_o:
            if(c == 'r') *state = STATE_PROLOG_r;
            else return STATE_ERROR;
        break;
        case STATE_PROLOG_r:
            if(c == 't') *state = STATE_PROLOG_t;
            else return STATE_ERROR;
        break;
        case STATE_PROLOG_t:
            token->type = TOKEN_PROLOG;
            return STATE_OK;
        break;
        default:
            return STATE_ERROR;
        break;
    }
    return STATE_OK;
}


State CheckForNullType(Token* token)
{
    if(!strcmp(token->data, "?i32"))
    {
        token->type = TOKEN_i32_NULL;
        return STATE_OK;
    }
    else if(!strcmp(token->data, "?f64"))
    {
        token->type = TOKEN_f64_NULL;
        return STATE_OK;
    }
    else if(!strcmp(token->data, "?[]u8"))
    {
        token->type = TOKEN_u8_NULL;
        return STATE_OK;
    }
    return STATE_ERROR;
}

void CheckKeyword(Token* token)
{
    if(!strcmp(token->data, "const"))
    {
        token->type = TOKEN_const;
        return;
    }
    if(!strcmp(token->data, "else"))
    {
        token->type = TOKEN_else;
        return;
    }
    if(!strcmp(token->data, "fn"))
    {
        token->type = TOKEN_fn;
        return;
    }
    if(!strcmp(token->data, "if"))
    {
        token->type = TOKEN_if;
        return;
    }
    if(!strcmp(token->data, "i32"))
    {
        token->type = TOKEN_i32;
        return;
    }
    if(!strcmp(token->data, "f64"))
    {
        token->type = TOKEN_f64;
        return;
    }
    if(!strcmp(token->data, "null"))
    {
        token->type = TOKEN_null;
        return;
    }
    if(!strcmp(token->data, "pub"))
    {
        token->type = TOKEN_pub;
        return;
    }
    if(!strcmp(token->data, "return"))
    {
        token->type = TOKEN_return;
        return;
    }
    if(!strcmp(token->data, "[]u8"))
    {
        token->type = TOKEN_u8;
        return;
    }
    if(!strcmp(token->data, "var"))
    {
        token->type = TOKEN_var;
        return;
    }
    if(!strcmp(token->data, "void"))
    {
        token->type = TOKEN_void;
        return;
    }
    if(!strcmp(token->data, "while"))
    {
        token->type = TOKEN_while;
        return;
    }
}