#include <stdio.h>
#include <stdlib.h>
#include "token.h"

TokenList* InitTokenList()
{
    TokenList* list = (TokenList*)malloc(sizeof(TokenList));
    if (list == NULL) {
        fprintf(stderr, "TOKEN LIST: Memory allocation failed\n");
        exit(1);
    }
    list->currToken = NULL;
    list->firstToken = NULL;
    return list;
}

Token* InitToken()
{
    Token* token = (Token*)malloc(sizeof(Token));
    if (token == NULL) {
        fprintf(stderr, "TOKEN: Memory allocation failed\n");
        exit(1);
    }
    token->data=NULL;
    token->nextToken=NULL;
    token->prevToken=NULL;
    token->type=TOKEN_UNKNOWN;
    token->dataLength=0;
    return token;
}

void DataToToken(char c, Token* token)
{
    token->dataLength++;
    token->data = (char*)realloc(token->data, (token->dataLength + 1)*sizeof(char));
    token->data[token->dataLength-1] = c;
    token->data[token->dataLength] = '\0';
}

void GetNextToken(Token** token)
{
    Token* nextToken = InitToken();
    (*token)->nextToken = nextToken;
    nextToken->prevToken = *token;
    *token = nextToken;
}

void GetPrevToken(Token** token)
{
    Token* prevToken = InitToken();
    (*token)->prevToken = prevToken;
    prevToken->nextToken = *token;
    *token = prevToken;
}


void freeToken(Token* token)
{
    free(token->data);
    free(token);
}

void freeTokenList(TokenList* list)
{
    Token* current = list->firstToken;
    Token* next;

    while (current != NULL) {
        next = current->nextToken;
        free(current);
        current = next;

    }

    free(list);
}

void PrintTokenList(TokenList* list)
{
    Token* current = list->firstToken;

    while (current != NULL) {
        printf("Token Type: %s, Data: %s\n", TokenTypeString(current->type), current->data);
        
        current = current->nextToken;
    }
}

void PrintToken(Token* token)
{
    printf("Token Type: %s, Data: %s\n", TokenTypeString(token->type), token->data);
}

char* TokenTypeString(Tokentype type)
{
    switch (type)
    {
    case TOKEN_ERROR:
        return "TOKEN_ERROR";
    case TOKEN_UNKNOWN:
        return "TOKEN_UNKNOWN";
    case TOKEN_VARIABLE:
        return "TOKEN_VARIABLE";
    case TOKEN_UNDERSCORE:
        return "TOKEN_UNDERSCORE";
    case TOKEN_PROLOG:
        return "TOKEN_PROLOG";
    case TOKEN_const:
        return "TOKEN_const";
    case TOKEN_else:
        return "TOKEN_else";
    case TOKEN_fn:
        return "TOKEN_fn";
    case TOKEN_if:
        return "TOKEN_if";
    case TOKEN_i32:
        return "TOKEN_i32";
    case TOKEN_f64:
        return "TOKEN_f64";
    case TOKEN_u8_NULL:
        return "TOKEN_u8_NULL";
    case TOKEN_i32_NULL:
        return "TOKEN_i32_NULL";
    case TOKEN_f64_NULL:
        return "TOKEN_f64_NULL";
    case TOKEN_null:
        return "TOKEN_null";
    case TOKEN_pub:
        return "TOKEN_pub";
    case TOKEN_return:
        return "TOKEN_return";
    case TOKEN_u8:
        return "TOKEN_u8";
    case TOKEN_var:
        return "TOKEN_var";
    case TOKEN_void:
        return "TOKEN_void";
    case TOKEN_while:
        return "TOKEN_while";
    case TOKEN_INTEGER:
        return "TOKEN_INTEGER";
    case TOKEN_FLOAT:
        return "TOKEN_FLOAT";
    case TOKEN_STRING:
        return "TOKEN_STRING";
    case TOKEN_PLUS:
        return "TOKEN_PLUS";
    case TOKEN_MINUS:
        return "TOKEN_MINUS";
    case TOKEN_MUL:
        return "TOKEN_MUL";
    case TOKEN_DIV:
        return "TOKEN_DIV";
    case TOKEN_COMMENT:
        return "TOKEN_COMMENT";
    case TOKEN_EQUAL:
        return "TOKEN_EQUAL";
    case TOKEN_ASSIGN:
        return "TOKEN_ASSIGN";
    case TOKEN_LESSER:
        return "TOKEN_LESS";
    case TOKEN_LESSER_EQUAL:
        return "TOKEN_LESSER_EQUAL";
    case TOKEN_GREATER:
        return "TOKEN_GREATER";
    case TOKEN_GREATER_EQUAL:
        return "TOKEN_GREATER_EQUAL";
    case TOKEN_NOT_EQUAL:
        return "TOKEN_NOT_EQUAL";
    case TOKEN_LEFT_PAR:
        return "TOKEN_LEFT_PAR";
    case TOKEN_RIGHT_PAR:
        return "TOKEN_RIGHT_PAR";
    case TOKEN_CURLY_LEFT_PAR:
        return "TOKEN_CURLY_LEFT_PAR";
    case TOKEN_CURLY_RIGHT_PAR:
        return "TOKEN_CURLY_RIGHT_PAR";
    case TOKEN_SEMICOLON:
        return "TOKE_SEMICOLON";
    case TOKEN_COLON:
        return "TOKEN_COLON";
    case TOKEN_BAR:
        return "TOKEN_BAR";
    case TOKEN_COMMA:
        return "TOKEN_COMMA";
    case TOKEN_EOF:
        return "TOKEN_EOF";
    case TOKEN_EOL:
        return "TOKEN_EOL";
    case TOKEN_SPACE:
        return "TOKEN_SPACE";
    default:
        return "UNKNOWN";
    }
}
