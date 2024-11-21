/**
 * Project: IFJ24 2024
 * 
 * Vojtěch Pánek 
 * generovani adresy, parametru a buffer
 * 
 */
char buffer[1024] = ""; // To do donastavit velikost

void append_function(char* buffer,  char* function_id) {
    // Vytvor zformatovany retezec
    char label[256]; 
    snprintf(label, sizeof(label), "LABEL $%s\n", function_id);

    // připoj zformatovany retezec k bufferu
    strncat(buffer, label, 1024 - strlen(buffer) - 1);
}
void append_function_param(char* buffer,char *param_id)
{ 
    // Vytvor zformatovany retezec
    char param[256];  
    snprintf(param, sizeof(param), "DEFVAR LF@%s\n", param_id);

    //  připoj zformatovany retezec k bufferu
    strncat(buffer, param, 1024 - strlen(buffer) - 1);
	

	
}
