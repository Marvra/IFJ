/**
 * Author:  Vojtěch Pánek   <xpanekv00>
 * Generátor kodu IFJ2024
 *  */

#ifndef IFJ_PANEK_GENERATOR_H
#define IFJ_PANEK_GENERATOR_H

#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "symtable.h"
#include "error.h"
#include "string.h"

void generate_error();

void generator_start();

void generator_clear();

void generator_flush();

void generate_main_scope_start();

void generate_main_scope_end();

void generate_function_start();

void generate_function_end();

void generate_function_retval();

void generate_function_call();

void generate_function_retval_assign();

void generate_function_param_declare();

void generate_function_before_pass_params();

void generate_function_pass_param();

void generate_function_convert_passed_param();

void generate_function_return();

void generate_var_declare();

void generate_input();

void generate_print();

void generate_push();

void generate_stack_operation();

void generate_concat_stack_strings();

void generate_save_expression_result();

void generate_if_head();

void generate_if_start();

void generate_if_else_part();

void generate_if_end();

void generate_while_head();

void generate_while_start();

void generate_while_end();

void generate_error();

#endif 