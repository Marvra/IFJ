#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>

typedef enum Tokentype{
    TOKEN_ERROR,
    TOKEN_UNKOWN,
    TOKEN_VARIABLE,
    TOKEN_INTEGER,
    TOKEN_STRING,
    TOKEN_QUESTIONMARK,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_COMMENT,
    TOKEN_EQUAL,
    TOKEN_ASSIGN,
    TOKEN_LESSER,
    TOKEN_LESSER_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LEFT_PAR,
    TOKEN_RIGHT_PAR,
    TOKEN_CURLY_LEFT_PAR,
    TOKEN_CURLY_RIGHT_PAR,
    TOKE_SEMICOLON,
    TOKEN_COLON,
    TOKEN_BAR,
    TOKEN_COMMA,
    TOKEN_SPACE,
    TOKEN_FLOAT,
    TOKEN_EOL,
    TOKEN_EOF
} Tokentype;

typedef struct Token
{
    Tokentype type;
    char *data;
    int dataLength;
    struct Token *prevToken;
    struct Token *nextToken;
}Token;

typedef struct TokenList
{
    struct Token *currToken;
    struct Token *firstToken;
}TokenList;

void GetNextToken(struct Token** token);
void GetPrevToken(struct Token** token);
void PrintTokenList(struct TokenList* list);
void PrintToken(struct Token* token);
char* TokenTypeString(Tokentype type);
struct TokenList* InitTokenList();
struct Token* InitToken();
void DataToToken(char c, Token* token);
void freeToken(Token* token);
void freeTokenList(TokenList* list);

#endif