#include "ast.h"

void appendFunctionName(char *);
void appendFunctionParam(char*);
void dataType(char* , DataType, char*);
void varDeclaration(char*, DataType , char* ) ;
void assignment(char*);
void header();
void expression(char*, Operator);
void relationalContidion(char* , Operator);