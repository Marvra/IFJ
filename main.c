#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    TokenList* list = InitTokenList();
    int returnCode = 0;
    ASTNode* tree = NULL;

    //LEXICAL ANALYSIS
    returnCode = Lexer(stdin ,list);
    list->currToken = list->firstToken;

    if(returnCode != 0)
    {
        return returnCode;
    }

    // SYNATX ANALYSIS
    returnCode = Parser(&tree,list);

    if(returnCode != 0)
    {
        return returnCode;
    }
    freeTokenList(list);

    // SMEMANTIC ANALYSIS
    returnCode = SemanticAnalysis(tree);

    if(returnCode != 0)
    {
        return returnCode;
    }
    

    // CODE GENERATION
    ASTNode* currentNode = tree;
    CodeGen(currentNode);
    
    return returnCode;
}