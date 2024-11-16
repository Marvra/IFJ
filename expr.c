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

// Prevadi tokeny na termy, viz. tabulka
precTableTerm_t expr_getTermFromToken(Token *token)
{
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
    case TOKEN_INTEGER:
    case TOKEN_FLOAT:
      return TERM_variable;
    default:
      return TERM_stackEnd;
  }
}

int getIndexFromTerm(precTableTerm_t term) {
  switch (term)
  {
  case TERM_plusMinus:
    return 0;
  case TERM_mulDiv:
    return 1;
  case TERM_leftBracket:
    return 2;
  case TERM_rightBracket:
    return 3;
  case TERM_relational:
    return 4;
  case TERM_variable:
    return 5;
  case TERM_stackEnd:
    return 6;
  default:
    return -1;
  }
}

int expr_start(TokenList *list)
{
  DLList* linked_list = DLLInit();
  DLLInsertLast(linked_list, TERM_stackEnd, NON_T_EXPR);

  int col = -1;
  int row = -1;

  while (1) 
  {
    row = linked_list->lastElement;
    DLLInsertLast(linked_list, expr_getTermFromToken(list->currToken), NON_T_EXPR);
    col = linked_list->lastElement;
    char sign = precTable[row][col];
    switch (sign)
    {
      case '>':

        list = list->currToken->nextToken;
        break;
      case '<':

        list = list->currToken->nextToken;
        break;
      case '=':

        list = list->currToken->nextToken;
        break;
      case '#':

        list = list->currToken->nextToken;
        break;
    }
  }
  DLLDispose(linked_list);
}
