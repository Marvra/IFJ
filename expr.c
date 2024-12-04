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

void skipWhitespaces(TokenList* list)
{
  while (list->currToken->type == TOKEN_SPACE || list->currToken->type == TOKEN_COMMENT || list->currToken->type == TOKEN_EOL)
  {
    list->currToken = list->currToken->nextToken;
  }

  return;
}

int checkForTop(TokenList *list, Tokentype topOnParserStack)
{
  Token* tempToken = list->currToken; 

  if (topOnParserStack == TOKEN_RIGHT_PAR && tempToken->type == TOKEN_RIGHT_PAR)
  {
    while (tempToken != NULL)
    {
      Tokentype currType = tempToken->nextToken->type;

      while (currType == TOKEN_SPACE || currType == TOKEN_COMMENT || currType == TOKEN_EOL)
      {
        tempToken = tempToken->nextToken;
        currType = tempToken->type;
      }

      if (currType == TOKEN_CURLY_LEFT_PAR || currType == TOKEN_BAR)
      {
        return 1;
      } 
      else 
      {
        return 0;
      }
    }
  }
  return 0;
}

int checkForFunction(TokenList *list, ASTNode **root, char *funcId) 
{
  // if ifj.func()
  if (list->currToken->type == TOKEN_DOT) 
  {
    strcat(funcId, list->currToken->data); // get dot
    list->currToken = list->currToken->nextToken;
    skipWhitespaces(list);
    strcat(funcId, list->currToken->data); // get after fot
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
    //CreateCodeNode(*root); // create code node
    *root = CreateFunCallExpressionsNode(*root); // create function call node
    *root = CreateIdNode(*root, funcId); // create id node

    while (list->currToken != NULL) {
      list->currToken = list->currToken->nextToken;
      skipWhitespaces(list);
      if (list->currToken->type == TOKEN_RIGHT_PAR)
      {
        return 0;
      }
      else if (list->currToken->type == TOKEN_INTEGER || list->currToken->type == TOKEN_FLOAT || list->currToken->type == TOKEN_STRING || list->currToken->type == TOKEN_VARIABLE)
      {
        tokenToParameter(list->currToken, &(*root));
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
  char *funcId = NULL;
  DLLInsertLast(linked_list, TERM_stackEnd);
  precTableTerm_t currTerm = NO_TERM_E;
  int incomingToken = -1;
  int topToken = -1;
  char tableSign = ' ';

  Token* curr_token = (*list)->currToken;

  if((*list)->currToken->type == TOKEN_null)
  {
    (*root) = CreateAstNode(TYPE_NULL);
    (*list)->currToken = (*list)->currToken->nextToken;
    skipWhitespaces(*list);
    return 0;
  }
  if ((*list)->currToken->type == TOKEN_VARIABLE) 
  {
    // get function name
    funcId = strdup((*list)->currToken->data);

    (*list)->currToken = (*list)->currToken->nextToken;
    skipWhitespaces(*list);
    //PrintToken((*list)->currToken);
    if ((*list)->currToken->type != TOKEN_DOT && (*list)->currToken->type != TOKEN_LEFT_PAR) 
    {
      (*list)->currToken = curr_token;
    }
    else if(checkForFunction(*list, &(*root), funcId))
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

  while (CheckForEnd(*linked_list))
  {
    skipWhitespaces(*list);

    currTerm = expr_getTermFromToken((*list)->currToken);

    incomingToken = getIndexFromTerm(currTerm);
    topToken = listTopIndex(*linked_list);
    tableSign = precTable[topToken][incomingToken];


    if (checkForTop(*list, topOnParserStack) && tableSign != '>')
    { 
      incomingToken = getIndexFromTerm(TERM_stackEnd);
      topToken = listTopIndex(*linked_list);
      tableSign = precTable[topToken][incomingToken];
    }

    //DLLPrintTerms(linked_list);

    if(tableSign == '<')
    {
      insert(root, (*list)->currToken);
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
      insert(root, (*list)->currToken);
      DLLInsertLast(linked_list, currTerm);
      if(CheckRule(linked_list))
      {
        DLLDispose(linked_list);
        return 1;
      }
      (*list)->currToken = (*list)->currToken->nextToken;
      continue;
    }
    else if (tableSign == '#')
    {
      DLLDispose(linked_list);
      return 1;
    }
    else if (tableSign == '@')
    {
      if(CheckRule(linked_list))
      {
        DLLDispose(linked_list);
        return 1;
      }
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

  if(data2 == NULL)
  {
    return 1;
  }

  if (data2->termType == TERM_stackEnd && data1->termType == NO_TERM_E) // E -> $E <---- THIS IS END CHECK
  {
    DLLInsertLast(linked_list, NO_TERM_E);
    return 0;
  }

  data3 = DLLDeleteLast(linked_list);

  if (data3 == NULL)
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
  DLLElementPtr current = linked_list.lastElement;
  while(current != NULL && (current->data->termType == NO_TERM_E)) 
  {
    current = current->previousElement;
  }
  if(current != NULL) {
    return getIndexFromTerm(current->data->termType);
  }
  return getIndexFromTerm(linked_list.lastElement->data->termType);
}

// --------------------------------- AST CREATION ---------------------------------
void insert(ASTNode **root, Token *curr_token) {
  if (expr_getTermFromToken(curr_token) == TERM_variable) 
  {
    ASTNode *new_node = NULL;

    if (curr_token->type == TOKEN_INTEGER) 
    {
      new_node = CreateAstNode(TYPE_VALUE_I32);
      new_node->data.i32 = atoi(curr_token->data);
    } 
    else if (curr_token->type == TOKEN_FLOAT) 
    {
      new_node = CreateAstNode(TYPE_VALUE_F64);
      new_node->data.f64 = atof(curr_token->data);
    } 
    else if (curr_token->type == TOKEN_VARIABLE) 
    {
      new_node = CreateAstNode(TYPE_ID);
      new_node->data.str = strdup(curr_token->data);
    } 
    else if (curr_token->type == TOKEN_STRING) 
    {
      new_node = CreateAstNode(TYPE_STRING);
      new_node->data.str = strdup(curr_token->data);
    }

    if (*root == NULL) 
    {
      *root = new_node;
    } 
    else 
    {
      ASTNode *current = *root;
      while (current->right != NULL) 
      {
        current = current->right;
      }
      current->right = new_node;
    }
  } 
  else if (expr_getTermFromToken(curr_token) != TERM_leftBracket && expr_getTermFromToken(curr_token) != TERM_rightBracket)
  {
    ASTNode *new_node = NULL;
    if (curr_token->type == TOKEN_PLUS || curr_token->type == TOKEN_MINUS || curr_token->type == TOKEN_MUL || curr_token->type == TOKEN_DIV)
    {
      new_node = CreateAstNode(TYPE_OPERATOR);
    } 
    else 
    {
      new_node = CreateAstNode(TYPE_REL_OPERATOR);
    }
    new_node->data.op = getOperatorFromToken(curr_token->type);

    int new_precedence = getTokenPrecedance(curr_token->type);

    if (*root == NULL) 
    {
      *root = new_node;
      return;
    }

    if (getAstPrecedance(*root) < new_precedence) 
    {
      new_node->left = *root;
      *root = new_node;
    } 
    else 
    {
      ASTNode *current = *root;

      while (current->right != NULL && getAstPrecedance(current->right) >= new_precedence) 
      {
        current = current->right;
      }

      new_node->left = current->right;
      current->right = new_node;
    }
  }
}
void tokenToParameter(Token *token, ASTNode **node) 
{
  if (token->type == TOKEN_INTEGER)
  {
    CreateArgumentNodeI32(*node, atoi(token->data));
  }
  else if (token->type == TOKEN_FLOAT)
  {
    CreateArgumentNodeF64(*node, atof(token->data));
  }
  else if (token->type == TOKEN_STRING)
  {
    CreateArgumentNodeU8(*node, token->data);
  }
  else if (token->type == TOKEN_VARIABLE)
  {
    CreateArgumentNode(*node, token->data);
  }
}

int getAstPrecedance(ASTNode *root)
{
  if (root->type == TYPE_OPERATOR || root->type == TYPE_REL_OPERATOR)
  {
    switch (root->data.op)
    {
      case OP_MUL:
      case OP_DIV:
        return 1;
      case OP_ADD:
      case OP_SUB:
        return 2;
      case OP_EQ:
      case OP_NEQ:
      case OP_LESS:
      case OP_GREATER:
      case OP_LE:
      case OP_GE:
        return 3;
      default:
        return -1;
    }
  }
  return -1;
}

int getTermPrecedence(precTableTerm_t term) 
{
    switch (term) {
        case TERM_plusMinus:
            return 1;
        case TERM_mulDiv:
            return 2;
        case TERM_relational:
            return 3;
        default:
            return -1;
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
