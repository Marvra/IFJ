/**
 * @file token.c
 * @author Martin Vrablec
 * @brief  source file for creating double linked list of tokens
 * @todo
 */
#include <stdio.h>
#include <stdlib.h>
#include "token.h"

/**
 * @brief creates empty token list
 * @return TokenList* pointer to token list
 */
TokenList* InitTokenList()
{
    TokenList* list = (TokenList*)malloc(sizeof(TokenList));
    if (list == NULL) {
        exit(99);
    }
    list->currToken = NULL;
    list->firstToken = NULL;
    return list;
}

/**
 * @brief creates empty token
 * @return Token* pointer to token
 */
Token* InitToken()
{
    Token* token = (Token*)malloc(sizeof(Token));
    if (token == NULL) {
        exit(99);
    }
    token->data=NULL;
    token->nextToken=NULL;
    token->prevToken=NULL;
    token->type=TOKEN_UNKNOWN;
    token->dataLength=0;
    return token;
}

/**
 * @brief adds character to token data
 * @param c character to add
 * @param token referance to token
 */
void DataToToken(char c, Token* token)
{
    token->dataLength++;
    token->data = (char*)realloc(token->data, (token->dataLength + 1)*sizeof(char));
    token->data[token->dataLength-1] = c;
    token->data[token->dataLength] = '\0';
}

/**
 * @brief creates new token and sets it as next token
 * @param token referance to token
 */
void GetNextToken(Token** token)
{
    Token* nextToken = InitToken();
    (*token)->nextToken = nextToken;
    nextToken->prevToken = *token;
    *token = nextToken;
}

/**
 * @brief creates new token and sets it as previous token
 * @param token referance to token
 */
void GetPrevToken(Token** token)
{
    Token* prevToken = InitToken();
    (*token)->prevToken = prevToken;
    prevToken->nextToken = *token;
    *token = prevToken;
}

/**
 * @brief frees token data and token
 * @param token referance to token
 */
void freeToken(Token* token)
{
    free(token->data);
    free(token);
}

/**
 * @brief frees token list and all tokens
 * @param list referance to token list
 */
void freeTokenList(TokenList* list)
{
    Token* current = list->firstToken;
    Token* next;

    // while to free are toknes in list 
    while (current != NULL) {
        next = current->nextToken;
        free(current);
        current = next;

    }

    free(list);
}