/**
 * Project: IFJ24 2024
 * 
 * Vojtěch Pánek 

 * 
 */
//*************************************** */
//věci okolo funkcí
void append_function(char* function_id) {
    printf("LABEL $%s\n", function_id);
}
void append_function_param(char *param_id)
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