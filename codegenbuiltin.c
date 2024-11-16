/**
 * Project: Implementace builtin fcí pro ifj 24
 * @brief Code generator implementation.
 *
 * @author Vojtěch Pánek
 */

/// built-in functions

// pub fn ifj.readstr() ?[]u8
#define FUNCTION_Readstr													\
	"\n # Built-in function readstr"											\
    "\n LABEL ifj.readstr"
    "\n PUSHFRAME"
    "\n DEFVAR LF@result"
    "\n READ LF@result string"
    "\n RETURN LF@result"
    "\n POPFRAME"
//pub fn ifj.readi32() ?i32
#define FUNCTION_Readi
    "\n # Built-in function readi"
    "\n LABEL ifj.readi32"
    "\n PUSHFRAME"
    "\n DEFVAR LF@input"
    "\n DEFVAR LF@result"
    "\n READ LF@input int"
    "\n JUMPIFEQ ifj.readi32_null LF@input nil"
    "\n MOVE LF@result LF@input"
    "\n RETURN LF@result"
    "\n LABEL ifj.readi32_null"
    "\n MOVE LF@result nil"
    "\n RETURN LF@result"
    "\n POPFRAME"


// pub fn ifj.readf64() ?f64
#define FUNCTION_Readf
    "\n # Built-in function readf"
    "\n    LABEL ifj.readf64"
    "\n    PUSHFRAME"
    "\n    DEFVAR LF@input"
    "\n    DEFVAR LF@result"
    "\n    READ LF@input float"
    "\n    JUMPIFEQ ifj.readf64_null LF@input nil"
    "\n    MOVE LF@result LF@input"
    "\n    RETURN LF@result"
    "\n    LABEL ifj.readf64_null"
    "\n    MOVE LF@result nil"
    "\n   RETURN LF@result"
    "\n    POPFRAME"
    
//pub fn ifj.write(term) void

#define FUNCTION_write
    "\n # Built-in function write"
    "\n LABEL ifj.write"
    "\n PUSHFRAME"
    "\n DEFVAR LF@term"
    "\n MOVE LF@term LF@%1"
    "\n WRITE LF@term"
    "\n POPFRAME"
// pub fn ifj.i2f(term ∶ i32) f64

#define FUNCTION_i2f
 "\n # Built-in function i2f"
"\n LABEL ifj.i2f"
"\n PUSHFRAME"
"\n DEFVAR LF@result"
"\n INT2FLOAT LF@result LF@%1"
"\n RETURN LF@result"
"\n POPFRAME"

// pub fn ifj.f2i(term ∶ i64) f32

#define FUNCTION_f2i
"\n # Built-in function f2i"
"\n LABEL ifj.f2i"
"\n PUSHFRAME"
"\n DEFVAR LF@result"
"\n FLOAT2INT LF@result LF@%1"
"\n RETURN LF@result"
"\n POPFRAME"

// pub fn ifj.length(𝑠 : []u8) i32 
#define FUNCTION_length
"\n # Built-in function length"
"\n LABEL ifj.length"
"\n PUSHFRAME"
"\n DEFVAR LF@result"
"\n STRLEN LF@result LF@%1"
"\n RETURN LF@result"
"\n POPFRAME"

//pub fn ifj.concat(𝑠1 : []u8, 𝑠2 : []u8) []u8
#define FUNCTION_concat
"\n # Built-in function concat"
"\n LABEL ifj.concat"
"\n PUSHFRAME"
"\n DEFVAR LF@result"
"\n CONCAT LF@result LF@%1 LF@%2"
"\n RETURN LF@result"
"\n POPFRAME"
//pub fn ifj.substring(𝑠 : []u8, 𝑖 : i32, 𝑗 : i32) ?[]u8
#define FUNCTION_substring
"\n # Built-in function substring"
"\n LABEL ifj.substring"
"\n PUSHFRAME"
"\n DEFVAR LF@result"
"\n DEFVAR LF@length"
"\n DEFVAR LF@index"

"\n STRLEN LF@length LF@%1"
"\n LT LF@index LF@%2 LF@length"
"\n JUMPIFEQ ifj.substring_invalid LF@index bool@false"

"\n GETSUBSTRING LF@result LF@%1 LF@%2 LF@%3"
"\n RETURN LF@result"

"\n LABEL ifj.substring_invalid"
"\n MOVE LF@result nil"
"\n RETURN LF@result"
"\n POPFRAME"
//pub fn ifj.strcmp(𝑠1 : []u8, 𝑠2 : []u8) i32
#define FUNCTION_comp
"\n # Built-in function compare"
"\n LABEL ifj.strcmp"
"\n PUSHFRAME"
"\n DEFVAR LF@result"
"\n STRCMP LF@result LF@%1 LF@%2"
"\n RETURN LF@result"
"\n POPFRAME"
// pub fn ifj.ord(𝑠 : []u8, 𝑖 : i32) i32
#define FUNCTION_ord
"\n # Built-in function ord"
"\n LABEL ifj.ord"
"\n PUSHFRAME"
"\n DEFVAR LF@result"
"\n STRI2INT LF@result LF@%1 LF@%2"
"\n JUMPIFEQ ifj.ord_out_of_bounds LF@result nil"
"\n RETURN LF@result"

"\n LABEL ifj.ord_out_of_bounds"
"\n MOVE LF@result int@0"
"\n RETURN LF@result"
"\n POPFRAME"
