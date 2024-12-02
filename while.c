/**
 * Project: IFJ24 2024
 * 
 * Vojtěch Pánek 

 * 
 */

void CreateWhile(ASTNode *node, int cond)
{
    printf("LABEL $WHILE$START%d\n", cond);
    RelOperator(node->left);
    printf("PUSHS bool@true\n");
    printf("JUMPIFNEQS $WHILE$END%d\n", cond);
    TraverseASTCodeGen(node->right);
    printf("JUMP $WHILE$START%d\n", cond);
    printf("LABEL $WHILE$END%d\n", cond);
}