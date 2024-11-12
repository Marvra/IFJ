#ifndef TOKEN_H
#define TOKEN_H
typedef enum Tokentype{
    TOKEN_ERROR,
    TOKEN_UNKNOWN,
    TOKEN_UNDERSCORE,
    TOKEN_VARIABLE,
    TOKEN_PROLOG,
    TOKEN_const,
    TOKEN_else,
    TOKEN_fn,
    TOKEN_if,
    TOKEN_i32,
    TOKEN_f64,
    TOKEN_null,
    TOKEN_pub,
    TOKEN_return,
    TOKEN_u8,
    TOKEN_var,
    TOKEN_void,
    TOKEN_while,
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
    TOKEN_SQUARE_LEFT_PAR,
    TOKEN_SQUARE_RIGHT_PAR,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_BAR,
    TOKEN_COMMA,
    TOKEN_DOT,
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

TokenList* InitTokenList();
Token* InitToken();
void DataToToken(char c, Token* token);
void GetNextToken(Token** token);
void GetPrevToken(Token** token);
void freeToken(Token* token);
void freeTokenList(TokenList* list);
void PrintTokenList(TokenList* list);
void PrintToken(Token* token);
char* TokenTypeString(Tokentype type);

#endif