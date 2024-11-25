/**
 * Project: IFJ24 2024
 * 
 * Vojtěch Pánek 

 * 
 */

 //****************************************/
/// built-in functions

// pub fn ifj.readstr() ?[]u8

#define FUNCTION_Readstr \
  "\n LABEL $readstr" \
  "\n CREATEFRAME" \
  "\n PUSHFRAME" \
  "\n DEFVAR LF@out" \
  "\n READ LF@out string" \
  "\n PUSHS LF@out" \
  "\n POPFRAME" \
  "\n RETURN"

#define FUNCTION_Readi \
  "\n # Function: $readi32" \
  "\n LABEL $readi32" \
  "\n CREATEFRAME" \
  "\n PUSHFRAME" \
  "\n DEFVAR LF@out" \
  "\n DEFVAR LF@err" \
  "\n MOVE LF@err int@0" \
  "\n READ LF@out int" \
  "\n JUMPIFEQ $readi32_end LF@out nil@nil" \
  "\n PUSHS LF@out" \
  "\n PUSHS LF@err" \
  "\n POPFRAME" \
  "\n RETURN" \
  "\n LABEL $readi32_end" \
  "\n MOVE LF@err int@1" \
  "\n PUSHS nil@nil" \
  "\n PUSHS LF@err" \
  "\n POPFRAME" \
  "\n RETURN"

#define FUNCTION_Readf \
  "\n # Funkce: $readf64" \
  "\n LABEL $readf64" \
  "\n CREATEFRAME" \
  "\n PUSHFRAME" \
  "\n DEFVAR LF@out" \
  "\n DEFVAR LF@err" \
  "\n MOVE LF@err int@0" \
  "\n READ LF@out float" \
  "\n JUMPIFEQ $readf64_end LF@out nil@nil" \
  "\n PUSHS LF@out" \
  "\n PUSHS LF@err" \
  "\n POPFRAME" \
  "\n RETURN" \
  "\n LABEL $readf64_end" \
  "\n MOVE LF@err int@1" \
  "\n PUSHS nil@nil" \
  "\n PUSHS LF@err" \
  "\n POPFRAME" \
  "\n RETURN"

#define FUNCTION_write \
  "\n # Built-in function ifj_write" \
  "\n LABEL ifj_write" \
  "\n CREATEFRAME" \
  "\n PUSHFRAME" \
  "\n DEFVAR LF@term" \
  "\n MOVE LF@term LF@%1" \
  "\n JUMPIFEQ write_null LF@term nil@nil" \
  "\n DEFVAR LF@type" \
  "\n TYPE LF@type LF@term" \
  "\n JUMPIFEQ write_int LF@type string@int" \
  "\n JUMPIFEQ write_float LF@type string@float" \
  "\n JUMP write_end" \
  "\n LABEL write_int" \
  "\n WRITE LF@term" \
  "\n JUMP write_end" \
  "\n LABEL write_float" \
  "\n WRITE LF@term" \
  "\n JUMP write_end" \
  "\n LABEL write_null" \
  "\n WRITE string@null" \
  "\n LABEL write_end" \
  "\n POPFRAME" \
  "\n RETURN"

#define FUNCTION_i2f \
  "\n # Built-in function i2f" \
  "\n LABEL ifji2f" \
  "\n CREATEFRAME" \
  "\n PUSHFRAME" \
  "\n DEFVAR LF@result" \
  "\n INT2FLOAT LF@result LF@%1" \
  "\n PUSHS LF@result" \
  "\n POPFRAME" \
  "\n RETURN"

#define FUNCTION_f2i \
  "\n # Built-in function ifj.f2i" \
  "\n LABEL ifj_f2i" \
  "\n CREATEFRAME" \
  "\n PUSHFRAME" \
  "\n DEFVAR LF@result" \
  "\n FLOAT2INT LF@result LF@%1" \
  "\n PUSHS LF@result" \
  "\n POPFRAME" \
  "\n RETURN"

#define FUNCTION_string \
  "\n # Built-in function ifj.string" \
  "\n LABEL ifj_string" \
  "\n CREATEFRAME" \
  "\n PUSHFRAME" \
  "\n DEFVAR LF@result" \
  "\n MOVE LF@result LF@%1" \
  "\n PUSHS LF@result" \
  "\n POPFRAME" \
  "\n RETURN"

#define FUNCTION_length \
  "\n # Built-in function ifj.length" \
  "\n LABEL ifj_length" \
  "\n CREATEFRAME" \
  "\n PUSHFRAME" \
  "\n DEFVAR LF@result" \
  "\n STRLEN LF@result LF@%1" \
  "\n PUSHS LF@result" \
  "\n POPFRAME" \
  "\n RETURN"

#define FUNCTION_concat \
  "\n # Built-in function ifj.concat" \
  "\n LABEL ifj_concat" \
  "\n CREATEFRAME" \
  "\n PUSHFRAME" \
  "\n DEFVAR LF@result" \
  "\n CONCAT LF@result LF@%1 LF@%2" \
  "\n PUSHS LF@result" \
  "\n POPFRAME" \
  "\n RETURN"

#define FUNCTION_substring \
  "\n # Built-in function ifj.substring" \
  "\n LABEL ifj_substring" \
  "\n CREATEFRAME" \
  "\n PUSHFRAME" \
  "\n DEFVAR LF@result" \
  "\n DEFVAR LF@start" \
  "\n DEFVAR LF@end" \
  "\n DEFVAR LF@length" \
  "\n DEFVAR LF@temp" \
  "\n MOVE LF@start LF@%2" \
  "\n MOVE LF@end LF@%3" \
  "\n STRLEN LF@length LF@%1" \
  "\n LT LF@temp LF@start int@0" \
  "\n JUMPIFEQ substring_error LF@temp bool@true" \
  "\n LT LF@temp LF@end int@0" \
  "\n JUMPIFEQ substring_error LF@temp bool@true" \
  "\n GT LF@temp LF@start LF@length" \
  "\n JUMPIFEQ substring_error LF@temp bool@true" \
  "\n GT LF@temp LF@end LF@length" \
  "\n JUMPIFEQ substring_error LF@temp bool@true" \
  "\n GT LF@temp LF@start LF@end" \
  "\n JUMPIFEQ substring_error LF@temp bool@true" \
  "\n MOVE LF@i LF@start" \
  "\n MOVE LF@result string@" \
  "\n LABEL substring_loop" \
  "\n LT LF@temp LF@i LF@end" \
  "\n JUMPIFEQ substring_end LF@temp bool@false" \
  "\n GETCHAR LF@temp LF@%1 LF@i" \
  "\n CONCAT LF@result LF@result LF@temp" \
  "\n ADD LF@i LF@i int@1" \
  "\n JUMP substring_loop" \
  "\n LABEL substring_end" \
  "\n PUSHS LF@result" \
  "\n POPFRAME" \
  "\n RETURN" \
  "\n LABEL substring_error" \
  "\n PUSHS nil@nil" \
  "\n POPFRAME" \
  "\n RETURN"

#define FUNCTION_comp \
  "\n # Built-in function ifj.strcmp" \
  "\n LABEL ifj_strcmp" \
  "\n CREATEFRAME" \
  "\n PUSHFRAME" \
  "\n DEFVAR LF@result" \
  "\n DEFVAR LF@temp" \
  "\n STRCMP LF@temp LF@%1 LF@%2" \
  "\n JUMPIFEQ strcmp_equal LF@temp int@0" \
  "\n JUMPIFLT strcmp_less LF@temp int@0" \
  "\n MOVE LF@result int@1" \
  "\n JUMP strcmp_end" \
  "\n LABEL strcmp_less" \
  "\n MOVE LF@result int@-1" \
  "\n JUMP strcmp_end" \
  "\n LABEL strcmp_equal" \
  "\n MOVE LF@result int@0" \
  "\n LABEL strcmp_end" \
  "\n PUSHS LF@result" \
  "\n POPFRAME" \
  "\n RETURN"

#define FUNCTION_ord \
  "\n # Built-in function ifj.ord" \
  "\n LABEL ifj_ord" \
  "\n CREATEFRAME" \
  "\n PUSHFRAME" \
  "\n DEFVAR LF@result" \
  "\n DEFVAR LF@length" \
  "\n DEFVAR LF@index" \
  "\n DEFVAR LF@char" \
  "\n MOVE LF@index LF@%2" \
  "\n STRLEN LF@length LF@%1" \
  "\n LT LF@result LF@index int@0" \
  "\n JUMPIFEQ ord_error LF@result bool@true" \
  "\n GE LF@result LF@index LF@length" \
  "\n JUMPIFEQ ord_error LF@result bool@true" \
  "\n STRI2INT LF@result LF@%1 LF@index" \
  "\n PUSHS LF@result" \
  "\n POPFRAME" \
  "\n RETURN" \
  "\n LABEL ord_error" \
  "\n MOVE LF@result int@0" \
  "\n PUSHS LF@result" \
  "\n POPFRAME" \
  "\n RETURN"

#define FUNCTION_chr \
  "\n # Built-in function ifj.chr" \
  "\n LABEL ifj_chr" \
  "\n CREATEFRAME" \
  "\n PUSHFRAME" \
  "\n DEFVAR LF@result" \
  "\n INT2CHAR LF@result LF@%1" \
  "\n PUSHS LF@result" \
  "\n POPFRAME" \
  "\n RETURN"
// pub fn ifj.chr(i32) []u8
#define FUNCTION_chr \
"\n # Built-in function ifj.chr" \
"\n LABEL ifj_chr" \
"\n CREATEFRAME" \
"\n PUSHFRAME" \
"\n DEFVAR LF@result" \
"\n INT2CHAR LF@result LF@%1" \
"\n PUSHS LF@result" \
"\n POPFRAME" \
"\n RETURN"

//************************************************* */
//věci okolo funkcí
/* builtin fce*/
// Function to print FUNCTION_Readstr
void print_function_readstr() {
    printf("%s\n", FUNCTION_Readstr);
}

// Function to print FUNCTION_Readi
void print_function_readi() {
    printf("%s\n", FUNCTION_Readi);
}

// Function to print FUNCTION_Readf
void print_function_readf() {
    printf("%s\n", FUNCTION_Readf);
}

// Function to print FUNCTION_write
void print_function_write() {
    printf("%s\n", FUNCTION_write);
}

// Function to print FUNCTION_i2f
void print_function_i2f() {
    printf("%s\n", FUNCTION_i2f);
}

// Function to print FUNCTION_f2i
void print_function_f2i() {
    printf("%s\n", FUNCTION_f2i);
}

// Function to print FUNCTION_string
void print_function_string() {
    printf("%s\n", FUNCTION_string);
}

// Function to print FUNCTION_length
void print_function_length() {
    printf("%s\n", FUNCTION_length);
}

// Function to print FUNCTION_concat
void print_function_concat() {
    printf("%s\n", FUNCTION_concat);
}

// Function to print FUNCTION_substring
void print_function_substring() {
    printf("%s\n", FUNCTION_substring);
}

// Function to print FUNCTION_comp
void print_function_comp() {
    printf("%s\n", FUNCTION_comp);
}

// Function to print FUNCTION_ord
void print_function_ord() {
    printf("%s\n", FUNCTION_ord);
}

// Function to print FUNCTION_chr
void print_function_chr() {
    printf("%s\n", FUNCTION_chr);
}
/**************************** */
void append_function(function_id) {
    printf("LABEL $%s\n", function_id);
}
void append_function_param(param_id)
{ 
    printf("DEFVAR LF@%s\n",param_id);
	
}
void append_function_call(char *function_id)
{
	printf("LABEL $%s\n", function_id);
}

void generate_function_end(char *function_id)
{
	
    printf("LABEL $%s\n return\npopframe\nreturn", function_id);

}

void append_function_return(char *func_id)
{  
    printf("JUMP$%s\nreturn\n", func_id);	
}
//**************************************** */
//ify
void append_function_if_start(char* func_id, int label_index, int label_deep) {

    printf("JUMPIFEQ $%s%d%d GF@%exp_result bool@false\n", func_id, label_deep, label_index);

}

void append_function_if_else_part(char* func_id, int label_index, int label_deep) {
    printf("JUMP $%s%d%d\n", func_id, label_deep, label_index + 1);
    printf("LABEL $%s%d%d\n", func_id, label_deep, label_index);
}

void append_function_call(char *function_id)
{
	printf("LABEL $%s\n", function_id);
}

void generate_function_end(char *function_id)
{
	
    printf("LABEL $%s\n return\npopframe\nreturn", function_id);

}
//***************************************** */
// while