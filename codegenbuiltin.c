/**
 * Project: Implementace builtin fcí 2.0 pro ifj 24
 * @brief Code generator implementation.
 *
 * @author Vojtěch Pánek
 */

/// built-in functions

// pub fn ifj.readstr() ?[]u8

#define FUNCTION_Readstr													\
  "\n LABEL $readstr "
  "\n CREATEFRAME "         // # Vytvoření rámce pro funkci
  "\n PUSHFRAME  "          // # Nastavení rámce

  "\n DEFVAR LF@out  "      // # Lokální proměnná
  "\n READ LF@out string "  // # Načtení vstupu od uživatele
  "\n PUSHS LF@out "        // # Uložení výsledku na zásobník

  "\n POPFRAME "            // # Uvolnění rámce
  "\n RETURN   "            // # Návrat z funkce

//pub fn ifj.readi32() ?i32

#define FUNCTION_Readi
"\n # Function: $readi32"
"\n LABEL $readi32"
"\n CREATEFRAME  "      // # vytvor novy ramec 
"\n PUSHFRAME  "         //# pushni ramec na zasobnik ramcu

"\n DEFVAR LF@out "      //# Definuj 'out' v lokalnim ramci"
"\n DEFVAR LF@err "      //# Definuj  local 'err' v lokálním rámci
"\n MOVE LF@err int@0 " // # Initializuj 'err' 

"\n READ LF@out int "    //# přečti intger  z stind na out
"\n JUMPIFEQ $readi32_end LF@out nil@nil" //  # if out ==nill error
"\n PUSHS LF@out "       //# pushni out na stack
"\n PUSHS LF@err "       //# pushni err na stack
"\n POPFRAME    "        //# popni rameec z vrcholu zasobniku ramcu
"\n RETURN    "          //# navrat z fce

"\n # End label for the function"
"\n LABEL $readi32_end"
"\n MOVE LF@err int@1  "// # nastav err na 1
"\n PUSHS nil@nil "     // # pushni nil na zasobnik pro indikaci chyby
"\n PUSHS LF@err  "     // # pushni err na zasobnik
"\n POPFRAME  "          //# popni ramec z vrcholu zasobniku ramcu
"\n RETURN    "         // # navrat


// pub fn ifj.readf64() ?f64
#define FUNCTION_Readf

"\n # Funkce: $readf64"
"\n LABEL $readf64"
"\n CREATEFRAME  "       //# Vytvoření nového dočasného rámce pro funkci
"\n PUSHFRAME  "         //# Přesunutí dočasného rámce na zásobník rámců

"\n DEFVAR LF@out "      //# Definice lokální proměnné 'out' v lokálním rámci
"\n DEFVAR LF@err "      //# Definice lokální proměnné 'err' v lokálním rámci
"\n MOVE LF@err int@0 "
"\n READ LF@out float"   //# Načtení desetinného čísla ze standardního vstupu do 'out'
"\n JUMPIFEQ $readf64_end LF@out nil@nil"  //# Pokud je 'out' nil (indikace chyby nebo EOF), skoč na konec

"\n PUSHS LF@out "       //# Uložení hodnoty 'out' na zásobník
"\n PUSHS LF@err "       //# Uložení hodnoty 'err' na zásobník
"\n POPFRAME    "        //# Odstranění aktuálního rámce z vrcholu zásobníku rámců
"\n RETURN   "           //# Návrat z funkce

"\n # Konec funkce"
"\n LABEL $readf64_end"
"\n MOVE LF@err int@1"   //# Nastavení 'err' na 1 (indikace chyby)
"\n PUSHS nil@nil "      //# Uložení nil na zásobník pro indikaci chyby
"\n PUSHS LF@err  "      //# Uložení hodnoty 'err' na zásobník
"\n POPFRAME  "          //# Odstranění aktuálního rámce z vrcholu zásobníku rámců
"\n RETURN "            // # Návrat z funkce
    
//pub fn ifj.write(term) void

#define FUNCTION_write
"\n # Built-in function ifj_write"
"\n LABEL ifj_write"
"\n CREATEFRAME"         //# Vytvoření nového dočasného rámce pro funkci
"\n PUSHFRAME  "         //# Přesunutí dočasného rámce na zásobník rámců

"\n DEFVAR LF@term"      //# Definice lokální proměnné 'term'
"\n MOVE LF@term LF@%1"  //# Přesunutí hodnoty argumentu do 'term'

"\n # Kontrola, zda je term null"
"\n JUMPIFEQ write_null LF@term nil@nil"

"\n # Zjištění typu term"
"\n DEFVAR LF@type "    // # Definice lokální proměnné 'type'
"\n TYPE LF@type LF@term" //# Zjištění typu term a uložení do 'type'

"\n # Výpis hodnoty podle typu"
"\n JUMPIFEQ write_int LF@type string@int"
"\n JUMPIFEQ write_float LF@type string@float"
"\n JUMP write_end"

"\n # Výpis hodnoty typu int"
"\n LABEL write_int"
"\n WRITE LF@term"
"\n JUMP write_end"

"\n # Výpis hodnoty typu float"
"\n LABEL write_float"
"\n WRITE LF@term"
"\n JUMP write_end"

"\n # Výpis hodnoty null"
"\n LABEL write_null"
"\n WRITE string@null"

"\n # Konec funkce"
"\n LABEL write_end"
"\n POPFRAME "           //# Odstranění aktuálního rámce z vrcholu zásobníku rámců
"\n RETURN "            // # Návrat z funkce
// pub fn ifj.i2f(term ∶ i32) f64

#define FUNCTION_i2f
"\n # Built-in function i2f"
"\n LABEL ifji2f"
"\n CREATEFRAME  "      // # Vytvoření nového dočasného rámce pro funkci
"\n PUSHFRAME "          //# Přesunutí dočasného rámce na zásobník rámců

"\n DEFVAR LF@result "   //# Definice lokální proměnné 'result'
"\n INT2FLOAT LF@result LF@%1 "// # Konverze celočíselné hodnoty argumentu na desetinné číslo a uložení do 'result'

"\n PUSHS LF@result"    // # Uložení výsledku na zásobník
"\n POPFRAME    "        //# Odstranění aktuálního rámce z vrcholu zásobníku rámců
"\n RETURN    "          //# Návrat z funkce

// pub fn ifj.f2i(term ∶ i64) f32

#define FUNCTION_f2i
"\n # Built-in function ifj.f2i"
"\n LABEL ifj_f2i"
"\n CREATEFRAME"         // Vytvoření nového dočasného rámce pro funkci
"\n PUSHFRAME"           // Přesunutí dočasného rámce na zásobník rámců

"\n DEFVAR LF@result"    // Definice lokální proměnné 'result'
"\n FLOAT2INT LF@result LF@%1" // Konverze desetinné hodnoty argumentu na celé číslo a uložení do 'result'

"\n PUSHS LF@result"     // Uložení výsledku na zásobník
"\n POPFRAME"            // Odstranění aktuálního rámce z vrcholu zásobníku rámců
"\n RETURN"              // Návrat z funkce
//pub fn ifj.string(term) []u8
#define FUNCTION_string
"\n # Built-in function ifj.string"
"\n LABEL ifj_string"
"\n CREATEFRAME"         // Vytvoření nového dočasného rámce pro funkci
"\n PUSHFRAME"           // Přesunutí dočasného rámce na zásobník rámců

"\n DEFVAR LF@result"    // Definice lokální proměnné 'result'
"\n MOVE LF@result LF@%1" // Přesunutí hodnoty argumentu do 'result'

"\n PUSHS LF@result"     // Uložení výsledku na zásobník
"\n POPFRAME"            // Odstranění aktuálního rámce z vrcholu zásobníku rámců
"\n RETURN"              // Návrat z funkce

// pub fn ifj.length(𝑠 : []u8) i32 
#define FUNCTION_length
"\n # Built-in function ifj.length"
"\n LABEL ifj_length"
"\n CREATEFRAME"         // Vytvoření nového dočasného rámce pro funkci
"\n PUSHFRAME"           // Přesunutí dočasného rámce na zásobník rámců

"\n DEFVAR LF@result"    // Definice lokální proměnné 'result'
"\n STRLEN LF@result LF@%1" // Zjištění délky řetězce a uložení do 'result'

"\n PUSHS LF@result"     // Uložení výsledku na zásobník
"\n POPFRAME"            // Odstranění aktuálního rámce z vrcholu zásobníku rámců
"\n RETURN"              // Návrat z funkce

//pub fn ifj.concat(𝑠1 : []u8, 𝑠2 : []u8) []u8
#define FUNCTION_concat
"\n # Built-in function ifj.concat"
"\n LABEL ifj_concat"
"\n CREATEFRAME"         // Vytvoření nového dočasného rámce pro funkci
"\n PUSHFRAME"           // Přesunutí dočasného rámce na zásobník rámců

"\n DEFVAR LF@result"    // Definice lokální proměnné 'result'
"\n CONCAT LF@result LF@%1 LF@%2" // Konkatenace dvou řetězců a uložení do 'result'

"\n PUSHS LF@result"     // Uložení výsledku na zásobník
"\n POPFRAME"            // Odstranění aktuálního rámce z vrcholu zásobníku rámců
"\n RETURN"              // Návrat z funkce
//pub fn ifj.substring(𝑠 : []u8, 𝑖 : i32, 𝑗 : i32) ?[]u8
#define FUNCTION_substring
"\n # Built-in function ifj.substring"
"\n LABEL ifj_substring"
"\n CREATEFRAME"         // Vytvoření nového dočasného rámce pro funkci
"\n PUSHFRAME"           // Přesunutí dočasného rámce na zásobník rámců

"\n DEFVAR LF@result"    // Definice lokální proměnné 'result'
"\n DEFVAR LF@start"     // Definice lokální proměnné 'start'
"\n DEFVAR LF@end"       // Definice lokální proměnné 'end'
"\n DEFVAR LF@length"    // Definice lokální proměnné 'length'
"\n DEFVAR LF@temp"      // Definice lokální proměnné 'temp'

"\n MOVE LF@start LF@%2" // Přesunutí hodnoty argumentu 'i' do 'start'
"\n MOVE LF@end LF@%3"   // Přesunutí hodnoty argumentu 'j' do 'end'
"\n STRLEN LF@length LF@%1" // Zjištění délky řetězce a uložení do 'length'

"\n # Kontrola platnosti indexů"
"\n LT LF@temp LF@start int@0"
"\n JUMPIFEQ substring_error LF@temp bool@true"
"\n LT LF@temp LF@end int@0"
"\n JUMPIFEQ substring_error LF@temp bool@true"
"\n GT LF@temp LF@start LF@length"
"\n JUMPIFEQ substring_error LF@temp bool@true"
"\n GT LF@temp LF@end LF@length"
"\n JUMPIFEQ substring_error LF@temp bool@true"
"\n GT LF@temp LF@start LF@end"
"\n JUMPIFEQ substring_error LF@temp bool@true"

"\n # Vytvoření podřetězce"
"\n DEFVAR LF@i"
"\n MOVE LF@i LF@start"
"\n MOVE LF@result string@"
"\n LABEL substring_loop"
"\n LT LF@temp LF@i LF@end"
"\n JUMPIFEQ substring_end LF@temp bool@false"
"\n GETCHAR LF@temp LF@%1 LF@i"
"\n CONCAT LF@result LF@result LF@temp"
"\n ADD LF@i LF@i int@1"
"\n JUMP substring_loop"

"\n LABEL substring_end"
"\n PUSHS LF@result"
"\n POPFRAME"
"\n RETURN"

"\n LABEL substring_error"
"\n PUSHS nil@nil"
"\n POPFRAME"
"\n RETURN"
//pub fn ifj.strcmp(𝑠1 : []u8, 𝑠2 : []u8) i32
#define FUNCTION_comp
"\n # Built-in function ifj.strcmp"
"\n LABEL ifj_strcmp"
"\n CREATEFRAME"         
"\n PUSHFRAME"          

"\n DEFVAR LF@result"    
"\n DEFVAR LF@s1"        
"\n DEFVAR LF@s2"        
"\n DEFVAR LF@char1"     
"\n DEFVAR LF@char2"     
"\n DEFVAR LF@i"         
"\n DEFVAR LF@len1"      
"\n DEFVAR LF@len2"     
"\n DEFVAR LF@cond"      

"\n MOVE LF@s1 LF@%1"    
"\n MOVE LF@s2 LF@%2"    

"\n STRLEN LF@len1 LF@s1" 
"\n STRLEN LF@len2 LF@s2" 
"\n MOVE LF@i int@0"     

"\n LABEL strcmp_loop"
"\n LT LF@cond LF@i LF@len1" 
"\n JUMPIFEQ strcmp_end LF@cond bool@false" 
"\n LT LF@cond LF@i LF@len2" 
"\n JUMPIFEQ strcmp_end LF@cond bool@false" 

"\n GETCHAR LF@char1 LF@s1 LF@i" 
"\n GETCHAR LF@char2 LF@s2 LF@i" 
"\n JUMPIFNEQ strcmp_diff LF@char1 LF@char2" 

"\n ADD LF@i LF@i int@1" 
"\n JUMP strcmp_loop"     

"\n LABEL strcmp_diff"
"\n LT LF@cond LF@char1 LF@char2" 
"\n JUMPIFEQ strcmp_less LF@cond bool@true" 
"\n MOVE LF@result int@1" 
"\n JUMP strcmp_end"

"\n LABEL strcmp_less"
"\n MOVE LF@result int@-1" 
"\n JUMP strcmp_end"

"\n LABEL strcmp_end"
"\n LT LF@cond LF@len1 LF@len2" 
"\n JUMPIFEQ strcmp_less LF@cond bool@true" 
"\n GT LF@cond LF@len1 LF@len2" 
"\n JUMPIFEQ strcmp_greater LF@cond bool@true" 
"\n MOVE LF@result int@0" 

"\n LABEL strcmp_greater"
"\n MOVE LF@result int@1" 

"\n PUSHS LF@result"    
"\n POPFRAME"            
"\n RETURN"              
// pub fn ifj.ord(𝑠 : []u8, 𝑖 : i32) i32
#define FUNCTION_ord
"\n # Built-in function ifj.ord"
"\n LABEL ifj_ord"
"\n CREATEFRAME"         
"\n PUSHFRAME"           

"\n DEFVAR LF@result"   
"\n DEFVAR LF@str"      
"\n DEFVAR LF@index"    
"\n DEFVAR LF@char"     
"\n DEFVAR LF@strlen"    
"\n DEFVAR LF@cond"     

"\n MOVE LF@str LF@%1"   
"\n MOVE LF@index LF@%2" 

"\n STRLEN LF@strlen LF@str" 


"\n JUMPIFEQ ord_out_of_bounds LF@strlen int@0" 
"\n LT LF@cond LF@index LF@strlen" 
"\n JUMPIFEQ ord_out_of_bounds LF@cond bool@false" 
"\n LT LF@cond LF@index int@0" 
"\n JUMPIFEQ ord_out_of_bounds LF@cond bool@true" 

"\n GETCHAR LF@char LF@str LF@index" 
"\n STRI2INT LF@result LF@str LF@index" 
"\n JUMP ord_end"

"\n LABEL ord_out_of_bounds"
"\n MOVE LF@result int@0" 

"\n LABEL ord_end"
"\n PUSHS LF@result"    
"\n POPFRAME"            
"\n RETURN"              
// pub fn ifj.chr(i32) []u8
#define FUNCTION_chr
"\n # Built-in function ifj.chr"
"\n LABEL ifj_chr"
"\n CREATEFRAME"         // Vytvoření nového dočasného rámce pro funkci
"\n PUSHFRAME"           // Přesunutí dočasného rámce na zásobník rámců

"\n DEFVAR LF@result"    // Definice lokální proměnné 'result'
"\n INT2CHAR LF@result LF@%1" // Převod ASCII hodnoty na znak a uložení do 'result'

"\n PUSHS LF@result"     // Uložení výsledku na zásobník
"\n POPFRAME"            // Odstranění aktuálního rámce z vrcholu zásobníku rámců
"\n RETURN"              // Návrat z funkce

