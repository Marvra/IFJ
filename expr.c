/**
 * @file expr.c
 * @authors Vojtech Panek, Jaroslav Doktor
 * @brief  file for precedent analysis of expressions
 * @todo
 */
#include "expr.h"



char precTable[PREC_TABLE_SIZE][PREC_TABLE_SIZE] =
{
  //+    -    *    /    (    )    ==   !=   <    >    <=   =>   i    $
  {'>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '<', '>'},  // +
  {'>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '<', '>'},  // -
  {'>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '<', '>'},  // *
  {'>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '<', '>'},  // /
  {'<', '<', '<', '<', '<', '=', '<', '<', '<', '<', '<', '<', '<', '#'},  // (
  {'>', '>', '>', '>', '#', '>', '>', '>', '>', '>', '>', '>', '#', '>'},  // )
  {'<', '<', '<', '<', '<', '>', '#', '#', '#', '#', '#', '#', '<', '>'},  // ==
  {'<', '<', '<', '<', '<', '>', '#', '#', '#', '#', '#', '#', '<', '>'},  // !=
  {'<', '<', '<', '<', '<', '>', '#', '#', '#', '#', '#', '#', '<', '>'},  // <
  {'<', '<', '<', '<', '<', '>', '#', '#', '#', '#', '#', '#', '<', '>'},  // >
  {'<', '<', '<', '<', '<', '>', '#', '#', '#', '#', '#', '#', '<', '>'},  // <=
  {'<', '<', '<', '<', '<', '>', '#', '#', '#', '#', '#', '#', '<', '>'},  // >=
  {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '#', '>'},  // i
  {'<', '<', '<', '<', '<', '#', '<', '<', '<', '<', '<', '<', '<', '@'}   // $
};

void expr_shift() {
  // Comment this

}

void expr_reduce() {
  // Comment this

}

void expr_illegal() {
  // Comment this
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