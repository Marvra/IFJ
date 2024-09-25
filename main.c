#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "lexer.h"


int main(int argc, char *argv[])
{
    TokenList* list = InitTokenList();
    int returnCode = 0;

    if (argc != 2)
    {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    FILE *file = fopen(argv[1], "r");

    returnCode = Lexer(file ,list);

    if(returnCode != 0)
    {
        printf("Error in lexer : %d\n", returnCode);
        return returnCode;
    }

    freeTokenList(list);
    
    fclose(file);
    return returnCode;
}