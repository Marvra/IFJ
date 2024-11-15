/**
 * @file expr.c
 * @authors Vojtech Panek, Jaroslav Doktor
 * @brief  file for precedent analysis of expressions
 * @todo
 */
#include "expr.h"



char precTable[PREC_TABLE_SIZE][PREC_TABLE_SIZE] =
{
  //+-   */   (    )  rela.  i    $
  {'>', '<', '<', '>', '>', '<', '>'},  // +-
  {'>', '>', '<', '>', '>', '<', '>'},  // */
  {'<', '<', '<', '=', '<', '<', '#'},  // (
  {'>', '>', '#', '>', '>', '#', '>'},  // )
  {'<', '<', '<', '>', '#', '<', '>'},  // relational
  {'>', '>', '>', '>', '>', '#', '>'},  // i
  {'<', '<', '<', '#', '<', '<', '@'}   // $
};

precTableTerm_t expr_getTermFromToken(Token *token){
  switch (token->type) {
    case TOKEN_PLUS:
    case TOKEN_MINUS:
      return TERM_plusMinus;
    case TOKEN_MUL:
    case TOKEN_DIV:
      return TERM_mulDiv;
    case TOKEN_CURLY_LEFT_PAR:
      return TERM_leftBracket;
    case TOKEN_CURLY_RIGHT_PAR:
      return TERM_rightBracket;
    case TOKEN_EQUAL:
    case TOKEN_NOT_EQUAL:
    case TOKEN_LESSER:
    case TOKEN_GREATER:
    case TOKEN_LESSER_EQUAL:
    case TOKEN_GREATER_EQUAL:
      return TERM_relational;
    case TOKEN_VARIABLE:
      return TERM_variable;
    default:
      return TERM_stackEnd;
  }
}

int expr_start(TokenList *list)
{
  while (true) // struktura neni prazdna ig 
  {
    if(list->currToken->type == TOKEN_SPACE || list->currToken->type == TOKEN_COMMENT)
    {
      Token* nextToken = list->currToken->nextToken;
      list->currToken = nextToken;
      continue;
    }
    // some expere_rules(list->currToken->type)
    Token* nextToken = list->currToken->nextToken;
    list->currToken = nextToken;
  }
  
}
