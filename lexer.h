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
    STATE_VARIABLE,
    STATE_QUESTIONMARK,
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
    STATE_SEMICOLON,
    STATE_COLON,
    STATE_BAR,
    STATE_COMMA,
    STATE_SPACE,
    STATE_INTEGER,
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
    STATE_EOL,
    STATE_EOF
} State;


int Lexer(FILE* file, TokenList* list);
int FSM(FILE* file, Token* token);
State GetFirstState(char c);
void DataToToken(char c, Token* token);

#endif