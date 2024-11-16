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
    case TOKEN_LEFT_PAR:
      return TERM_leftBracket;
    case TOKEN_RIGHT_PAR:
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
    case TOKEN_VARIABLE:
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

int checkForTop(TokenList *list, Tokentype topOnParserStack)
{
  Token* originalToken = list->currToken;

  if (topOnParserStack == TOKEN_RIGHT_PAR && originalToken->type == TOKEN_RIGHT_PAR)
  {
    while (list->currToken != NULL)
    {
      Tokentype currType = list->currToken->type;
      
      if (currType == TOKEN_SPACE || currType == TOKEN_COMMENT || currType == TOKEN_EOL || currType == TOKEN_EOF)
      {
        list->currToken = list->currToken->nextToken;
        continue;
      }

      if (currType == TOKEN_CURLY_LEFT_PAR)
      {
        list->currToken = originalToken;
        return 0;
      }
      list->currToken = list->currToken->nextToken;
    }
  }

  list->currToken = originalToken;
  return 1;
}

int expr_start(TokenList **list, Tokentype topOnParserStack)
{
  DLList* linked_list = DLLInit();
  DLLInsertLast(linked_list, TERM_stackEnd);
  precTableTerm_t currTerm = NO_TERM_E;
  int incomingToken = -1;
  int topToken = -1;
  char tableSign = ' ';

  while (CheckForEnd(*linked_list))
  {

    // skip whitespaces
    while ((*list)->currToken->type == TOKEN_SPACE || (*list)->currToken->type == TOKEN_EOL || (*list)->currToken->type == TOKEN_COMMENT)
    {
      (*list)->currToken = (*list)->currToken->nextToken;
    }
    currTerm = expr_getTermFromToken((*list)->currToken);
    incomingToken = getIndexFromTerm(currTerm);
    topToken = listTopIndex(*linked_list);
    tableSign = precTable[topToken][incomingToken];

    // lespei bude dat if (checkForTop(*list, topOnParserStack)) a potom break;
    // a do while das len ze pokial nebude linked_list n $ nejako ig

    if(tableSign == '<')
    {
      DLLInsertLast(linked_list, currTerm);
      (*list)->currToken = (*list)->currToken->nextToken;
      continue;
    }
    else if (tableSign == '>')
    {
      if(CheckRule(linked_list))
      {
        DLLDispose(linked_list);
        return 1;
      }
    }
    else if (tableSign == '=')
    {
      (*list)->currToken  = (*list)->currToken->nextToken;
    }
    else if (tableSign == '#')
    {
      DLLDispose(linked_list);
      return 1;
    }
    else if (tableSign == '@')
    {
      break;
    }
  }

  DLLDispose(linked_list);
  return 0;
}

int CheckRule(DLList* linked_list)
{
  DLLData data1, data2, data3;
  data1 = DLLDeleteLast(linked_list); 

  if(data1 == NULL)
  {
    return 1;
  }

  if(data1->termType == TERM_variable) // E -> i
  {
    DLLInsertLast(linked_list, NO_TERM_E);
    return 0;
  }

  data2 = DLLDeleteLast(linked_list);
  data3 = DLLDeleteLast(linked_list);

  if (data2 == NULL || data3 == NULL)
  {
    return 1;
  }
  

  if (data1->termType == NO_TERM_E && data2->termType == TERM_plusMinus && data3->termType == NO_TERM_E) // E -> E + E || E - E
  {
    DLLInsertLast(linked_list, NO_TERM_E);
    return 0;
  }
  else if (data1->termType == NO_TERM_E && data2->termType == TERM_mulDiv && data3->termType == NO_TERM_E) // E -> E * E || E / E
  {
    DLLInsertLast(linked_list, NO_TERM_E);
    return 0;
  }
  else if (data1->termType == NO_TERM_E && data2->termType == TERM_relational && data3->termType == NO_TERM_E) // E -> E == E || E != E || E >= ...
  {
    DLLInsertLast(linked_list, NO_TERM_E);
    return 0;
  }
  else if (data1->termType == TERM_leftBracket && data2->termType == NO_TERM_E && data3->termType == TERM_rightBracket) // E -> (E)
  {
    DLLInsertLast(linked_list, NO_TERM_E);
    return 0;
  }
  else
  {
    return 1;
  }
}

int CheckForEnd(DLList linked_list)
{
  if(linked_list.lastElement->data->termType == TERM_stackEnd && linked_list.currentLength > 1)
  {
    return 0;
  }
  return 1;
}

int listTopIndex(DLList linked_list)
{
  // ono to sice prejde vsetky NO_term_e ale prejde to aj pri tomto pripade (E) takze sa najde () coz posunie na dalsi token coz not good
  if(linked_list.lastElement->data->termType == NO_TERM_E)
  {
    return getIndexFromTerm(linked_list.lastElement->previousElement->data->termType);
  }
  return getIndexFromTerm(linked_list.lastElement->data->termType);
}
