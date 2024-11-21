#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    TokenList* list = InitTokenList();
    int returnCode = 0;
    ASTNode* tree = NULL;

    returnCode = Lexer(stdin ,list);
    list->currToken = list->firstToken;

    if(returnCode != 0)
    {
        //printf("Error in lexer : %d\n", returnCode);
        return returnCode;
    }

    //printf("STARTING PARSER\n");
    returnCode = Parser(&tree,list);

    if(returnCode != 0)
    {
        //printf("Error in Parser : %d\n", returnCode);
        return returnCode;
    }
    freeTokenList(list);

    // SMEMANTIC ANALYSIS

    //printf("STARTING SEMANTIC ANALYSIS\n");
    returnCode = SemanticAnalysis(tree);

    if(returnCode != 0)
    {
        //printf("Error in Semantic : %d\n", returnCode);
        return returnCode;
    }

    printf(".IFJcode24\n");
    
    return returnCode;
}