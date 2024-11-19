#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token.h"

typedef enum State{
    STATE_ERROR,
    STATE_START,
    STATE_OK,
    STATE_UNDERSCORE,
    STATE_VARIABLE,
    STATE_PLUS,
    STATE_MINUS,
    STATE_MUL,
    STATE_DIV,
    STATE_COMMENT,
    STATE_EQUAL,
    STATE_ASSIGN,
    STATE_LESSER,
    STATE_LESSER_EQUAL,
    STATE_GREATER,
    STATE_GREATER_EQUAL,
    STATE_NOT,
    STATE_NOT_EQUAL,
    STATE_LEFT_PAR,
    STATE_RIGHT_PAR,
    STATE_CURLY_LEFT_PAR,
    STATE_CURLY_RIGHT_PAR,
    STATE_SQUARE_LEFT_PAR,
    STATE_SQUARE_RIGHT_PAR,
    STATE_SEMICOLON,
    STATE_COLON,
    STATE_BAR,
    STATE_COMMA,
    STATE_DOT,
    STATE_SPACE,
    STATE_INTEGER,
    STATE_ZERO,
    STATE_FLOAT_DOT,
    STATE_FLOAT_DIGIT,
    STATE_FLOAT_EXP,
    STATE_FLOAT_EXP_PM,
    STATE_FLOAT_END,
    STATE_STRING_START,
    STATE_STRING_SLASH,
    STATE_STRING_HEX1,
    STATE_STRING_HEX2,
    STATE_STRING_END,
    STATE_PROLOG,
    STATE_PROLOG_i,
    STATE_PROLOG_m,
    STATE_PROLOG_p,
    STATE_PROLOG_o,
    STATE_PROLOG_r,
    STATE_PROLOG_t,
    STATE_NULL_START,
    STATE_NULL_RBRACKET,
    STATE_NULL_TYPE,
    STATE_EOL,
    STATE_EOF
} State;


int Lexer(FILE* file, TokenList* list);
int FSM(FILE* file, Token* token);
State GetFirstState(char c);
void CheckKeyword(Token* token);
int CheckForNullType(Token* token);

#endif // LEXER_H 