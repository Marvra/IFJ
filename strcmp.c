#define FUNCTION_comp \
"LABEL ifj_strcmp\n" \
"PUSHFRAME \n" \

"DEFVAR LF@param1\n" \
"MOVE LF@param1 LF@%1\n" \
"DEFVAR LF@param2\n" \
"MOVE LF@param2 LF@%2\n" \

"DEFVAR LF@result\n" \
"DEFVAR LF@len1\n" \
"DEFVAR LF@len2\n" \

"STRLEN LF@len1 LF@param1\n" \
"STRLEN LF@len2 LF@param2\n" \

"LT LF@result LF@len1 LF@len2\n" \
"JUMPIFEQ $set_minus_one LF@result bool@true\n" \

"GT LF@result LF@len1 LF@len2\n" \
"JUMPIFEQ $set_one LF@result bool@true\n" \

"MOVE LF@result int@0\n" \
"JUMP end\n" \
"LABEL $set_minus_one\n" \
"MOVE LF@result int@-1\n" \
"JUMP end\n" \
"LABEL $set_one\n" \
"MOVE LF@result int@1\n" \
"LABEL end\n" \
"PUSHS LF@result\n" \
"POPFRAME\n" \
"RETURN\n\n"