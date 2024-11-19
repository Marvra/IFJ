/**
 * @file expr.c
 * @authors Jaroslav Doktor, Martin Vrablec
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
    case TOKEN_STRING:
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

void skipWhitespaces(TokenList *list)
{
  while (list->currToken->type == TOKEN_SPACE || list->currToken->type == TOKEN_COMMENT || list->currToken->type == TOKEN_EOL)
  {
    list->currToken = list->currToken->nextToken;
  }

  return;
}

int checkForTop(TokenList *list, Tokentype topOnParserStack)
{
  Token* originalToken = list->currToken;

  if (topOnParserStack == TOKEN_RIGHT_PAR && originalToken->type == TOKEN_RIGHT_PAR)
  {
    while (list->currToken != NULL)
    {
      Tokentype currType = list->currToken->nextToken->type;
      
      skipWhitespaces(list);

      if (currType == TOKEN_CURLY_LEFT_PAR)
      {
        list->currToken = originalToken;
        return 1;
      } 
      else {
        return 0;
      }
    }
  }

  list->currToken = originalToken;
  return 0;
}

int checkForFunction(TokenList *list) 
{
  // if ifj.func()
  list->currToken = list->currToken->nextToken;
  skipWhitespaces(list);
  if (list->currToken->type == TOKEN_DOT) 
  {
    list->currToken = list->currToken->nextToken;
    skipWhitespaces(list);
    if (list->currToken->type != TOKEN_VARIABLE) 
    {
      return 1;
    }
    list->currToken = list->currToken->nextToken;
    skipWhitespaces(list);
  }
  // if func()
  if (list->currToken->type == TOKEN_LEFT_PAR)
  {
    while (list->currToken != NULL) {
      list->currToken = list->currToken->nextToken;
      skipWhitespaces(list);
      if (list->currToken->type == TOKEN_RIGHT_PAR)
      {
        return 0;
      }
      else if (list->currToken->type == TOKEN_INTEGER || list->currToken->type == TOKEN_FLOAT || list->currToken->type == TOKEN_STRING || list->currToken->type == TOKEN_VARIABLE)
      {
        list->currToken = list->currToken->nextToken;
        skipWhitespaces(list);
        if (list->currToken->type == TOKEN_RIGHT_PAR)
          return 0;
        else if (list->currToken->type == TOKEN_COMMA)
        {
          continue;
        }
        else 
        {
          return 1;
        }
      }
      else
      {
        return 1;
      }
    }
  }
  return 1;
}

int expr_start(ASTNode **root, TokenList **list, Tokentype topOnParserStack)
{
  DLList* linked_list = DLLInit();
  DLLInsertLast(linked_list, TERM_stackEnd);
  precTableTerm_t currTerm = NO_TERM_E;
  int incomingToken = -1;
  int topToken = -1;
  char tableSign = ' ';
  int didOperation = 0; // for counting operations to avoid while()
  // counting brackets
  int leftBrackets = 0; 
  int rightBrackets = 0;

  while (CheckForEnd(*linked_list))
  {
    PrintToken((*list)->currToken);

    skipWhitespaces(*list);

    // check if the function is called
    if ((*list)->currToken->type == TOKEN_VARIABLE) 
    {
      if(checkForFunction(*list))
      {
        return 1;
      }
      else 
      {
        (*list)->currToken = (*list)->currToken->nextToken;
        skipWhitespaces(*list);
        return 0;
      }
    }

    currTerm = expr_getTermFromToken((*list)->currToken);

    // count left & right brackets
    if (currTerm == TERM_leftBracket) 
    {
      leftBrackets++;
    }
    else if (currTerm == TERM_rightBracket)
    {
      rightBrackets++;
    }

    incomingToken = getIndexFromTerm(currTerm);
    topToken = listTopIndex(*linked_list);
    tableSign = precTable[topToken][incomingToken];

    // dealing here with while and if brackets
    if (checkForTop(*list, topOnParserStack)) { 
      if ((didOperation == 0) || (rightBrackets == leftBrackets)) // the brackets should never ==, because we have one more rightBracket 
      {
        return 1;
      }
      return 0;
    }


    if(tableSign == '<')
    {
      // insert(root, (*list)->currToken);
      DLLInsertLast(linked_list, currTerm);
      (*list)->currToken = (*list)->currToken->nextToken;
      didOperation++;
      continue;
    }
    else if (tableSign == '>')
    {
      didOperation++;
      if(CheckRule(linked_list))
      {
        DLLDispose(linked_list);
        return 1;
      }
    }
    else if (tableSign == '=')
    {
      DLLInsertLast(linked_list, currTerm);
      didOperation++;
      if(CheckRule(linked_list))
      {
        DLLDispose(linked_list);
        return 1;
      }
      (*list)->currToken = (*list)->currToken->nextToken;
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
  else if (data1->termType == TERM_rightBracket && data2->termType == NO_TERM_E && data3->termType == TERM_leftBracket) // E -> (E)
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

// --------------------------------- AST CREATION ---------------------------------
void insert(ASTNode **root, Token* curr_token) 
{
  if (expr_getTermFromToken(curr_token) == TERM_variable)
  { // Token is a number ONLY NUMBER
    ASTNode *new_node = NULL;

    // UGLY TYPE CHECK
    if(curr_token->type == TOKEN_INTEGER)
    {
      new_node = CreateAstNode(TYPE_VALUE_I32);
      new_node->data.i32 = atoi(strdup(curr_token->data));
    }
    else if(curr_token->type == TOKEN_FLOAT)
    {
      new_node = CreateAstNode(TYPE_VALUE_F64);
      new_node->data.f64 = atof(strdup(curr_token->data));
    }
    else if(curr_token->type == TOKEN_STRING)
    {
      new_node = CreateAstNode(TYPE_STRING);
      // new_node->data.str = strdup(curr_token->data);
    }
    else if(curr_token->type == TOKEN_VARIABLE)
    {
      new_node = CreateAstNode(TYPE_ID);
      new_node->data.str = strdup(curr_token->data);
    }
    // UGLY TYPE CHECK

    if (*root == NULL)
    {
      // First operand, make it the root
      *root = new_node;
    }
    else if ((*root)->right == NULL) 
    {
      // Attach as the right child of the current root
      (*root)->right = new_node;
    }
    else
    {
      fprintf(stderr, "Error: Unexpected token sequence (too many operands).\n");
      exit(1);
    }
  } 
  else 
  { // Token is an operator
    ASTNode *new_node = CreateAstNode(TYPE_OPERATOR);
    new_node->data.op = getOperatorFromToken(curr_token->type);
    if (*root == NULL)
    {
        // First operator, make it the root
        *root = new_node;
    }
    else 
    {
      int current_precedence = getAstPrecedance((*root)->type);
      int new_precedence = getTokenPrecedance(curr_token->type);
      
      if (new_precedence < current_precedence)
      {
          // Higher precedence: Make the new operator a subtree root
          new_node->left = (*root)->right;
          (*root)->right = new_node;
      }
      else
      {
          // Lower or equal precedence: Make the new operator the root
          new_node->left = *root;
          *root = new_node;
      }
    }
  }
}
int getTokenPrecedance(Tokentype type)
{
  switch (type)
  {
    case TOKEN_MUL:
    case TOKEN_DIV:
      return 1;
    case TOKEN_PLUS:
    case TOKEN_MINUS:
      return 2;
    case TOKEN_EQUAL:
    case TOKEN_NOT_EQUAL:
    case TOKEN_LESSER:
    case TOKEN_GREATER:
    case TOKEN_LESSER_EQUAL:
    case TOKEN_GREATER_EQUAL:
      return 3;
    default:
      return -1;
  }
}

int getAstPrecedance(ASTNodeType type)
{
  switch (type)
  {
    case TYPE_OPERATOR:
      return 1;
    case TYPE_VALUE_I32:
    case TYPE_STRING:
      return 0;
    default:
      return -1;
  }
}

Operator getOperatorFromToken(Tokentype type)
{
  switch (type)
  {
    case TOKEN_PLUS:
      return OP_ADD;
    case TOKEN_MINUS:
      return OP_SUB;
    case TOKEN_MUL:
      return OP_MUL;
    case TOKEN_DIV:
      return OP_DIV;
    case TOKEN_EQUAL:
      return OP_EQ;
    case TOKEN_NOT_EQUAL:
      return OP_NEQ;
    case TOKEN_LESSER:
      return OP_LESS;
    case TOKEN_GREATER:
      return OP_GREATER;
    case TOKEN_LESSER_EQUAL:
      return OP_LE;
    case TOKEN_GREATER_EQUAL:
      return OP_GE;
    default:
      return OP_DEFAULT;
  }
}
