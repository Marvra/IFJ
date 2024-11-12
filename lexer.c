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
    bool valid = true;
    Token* token = InitToken();
    list->firstToken = token;

    while (valid)
    {
        list->currToken = token;
        int fsmState = FSM(file, token);

        if(token->type == TOKEN_EOF)
        {
            printf("FSM found EOF\n");
            break;
        }
        
        if(fsmState == STATE_ERROR)
        {
            printf("Error in FSM\n");
            return ERROR_LEXER;
        }

        GetNextToken(&token);
    }

    PrintTokenList(list);
    return 0;
    
}

int FSM(FILE* file, Token* token)
{
    char c;
    State state = STATE_START;

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
                if(isdigit(c)) state = STATE_FLOAT_DIGIT;
                else return STATE_ERROR;
            break;

            case STATE_FLOAT_DIGIT:
                if(isdigit(c)) state = STATE_FLOAT_DIGIT;
                else if(c == 'e' || c == 'E') state = STATE_FLOAT_EXP;
                else token->type = TOKEN_FLOAT;
            break;

            case STATE_FLOAT_EXP:
                if(c == '+' || c == '-') state = STATE_FLOAT_EXP_PM;
                else if(isdigit(c)) state = STATE_FLOAT_END;
                else return STATE_ERROR;
            break;

            // TIETO 2 VZTAHY SA MOZNO DOKAZU DAT DO JEDNEHO  { STATE_FLOAT_EXP ---{+,-}---> STATE_FLOAT_END && ---{0-9}---> STATE_FLOAT_END }
            case STATE_FLOAT_EXP_PM:
                if(isdigit(c)) state = STATE_FLOAT_END;
                else return STATE_ERROR;
            break;

            case STATE_FLOAT_END:
                if(isdigit(c)) state = STATE_FLOAT_END;
                else token->type = TOKEN_FLOAT;
            break;
            // FLOAT END
            
            // STRING START
            case STATE_STRING_START:
                if(c == '"') state = STATE_STRING_END;
                else if(c == '\\') state = STATE_STRING_SLASH;
                //else if(c >=32 && c <= 127)  state = STATE_STRING_START;
                else if(c >=32)  state = STATE_STRING_START;
                else return STATE_ERROR;
            break;

            case STATE_STRING_SLASH:
                if(c == 'n' || c == 'r' || c == 't' || c == '\'' || c == '"' || c=='\\') state = STATE_STRING_START;
                else if(c == 'x') state = STATE_STRING_HEX1;
                else return STATE_ERROR;
            break;

            case STATE_STRING_HEX1:
                if(isxdigit(c)) state = STATE_STRING_HEX2;
                else return STATE_ERROR;
            break;
            
            case STATE_STRING_HEX2:
                if(isxdigit(c)) state = STATE_STRING_START;
                else return STATE_ERROR;
            break;

            case STATE_STRING_END:
                token->type = TOKEN_STRING;
            break;
            // STRING END

            // PROLOG START
            case STATE_PROLOG:
                if(c == 'i') state = STATE_PROLOG_i;
                else return STATE_ERROR;
            break;
            case STATE_PROLOG_i:
                if(c == 'm') state = STATE_PROLOG_m;
                else return STATE_ERROR;
            break;
            case STATE_PROLOG_m:
                if(c == 'p') state = STATE_PROLOG_p;
                else return STATE_ERROR;
            break;  
            case STATE_PROLOG_p:
                if(c == 'o') state = STATE_PROLOG_o;
                else return STATE_ERROR;
            break;
            case STATE_PROLOG_o:
                if(c == 'r') state = STATE_PROLOG_r;
                else return STATE_ERROR;
            break;
            case STATE_PROLOG_r:
                if(c == 't') state = STATE_PROLOG_t;
                else return STATE_ERROR;
            break;
            case STATE_PROLOG_t:
                token->type = TOKEN_PROLOG;
            break;
            // PROLOG END
            case STATE_QUESTIONMARK:
                token->type = TOKEN_QUESTIONMARK;
            break;
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
                // if(!(c == '\n' || c==EOF)) state == STATE_COMMENT;
                // else token->type = TOKEN_COMMENT;
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
            case STATE_SQUARE_LEFT_PAR:
                token->type = TOKEN_SQUARE_LEFT_PAR;
            break;
            case STATE_SQUARE_RIGHT_PAR:
                token->type = TOKEN_SQUARE_RIGHT_PAR;
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
                return STATE_ERROR;
            break;
        }

        if(token->type != TOKEN_UNKNOWN) 
        {
            if(token->type == TOKEN_VARIABLE)
            {
                CheckKeyword(token);
            }

            ungetc(c,file);
            return STATE_OK;
        }

        DataToToken(c, token);
    }
    return STATE_OK;
}
State GetFirstState(char c)
{
    if(isalpha(c)) return STATE_VARIABLE;
    else if(c=='_') return STATE_UNDERSCORE;
    else if(isdigit(c)) return STATE_INTEGER;
    else if(c=='"') return STATE_STRING_START;
    else if(c=='?') return STATE_QUESTIONMARK;
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
    else if(c==']') return STATE_SQUARE_RIGHT_PAR;
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
    if(!strcmp(token->data, "u8"))
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