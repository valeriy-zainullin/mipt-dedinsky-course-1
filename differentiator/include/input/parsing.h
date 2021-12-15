#pragma once

#include "reader.h"
#include "tree.h"

#include <stdbool.h>

// number = '0' | ('1'..'9' {'0'..'9'})
// OLD: // function = ("sin" | 'c' ("os" | "tg") | "tg" | "ln") '(' 1st_order_sequence ')'
// name = 'a'..'z' ['a'..'z' ['a'..'z' ... ] ]
//   Длина не более 16 (TREE_MAX_NAME_LENGTH).
// expression_in_braces = '(' 1st_order_sequence ')'
// value = ['+' | '-'] (number | name ['(' 1st_order_sequence ')'] | expression_in_braces)
//   Если есть скобки у name, значит это функция. Поддерживаются функции sin, cos, tg, ctg, ln
// 3rd_order_sequence = value
// 2nd_order_sequence = 3rd_order_sequence {('*' | '/') 3rd_order_sequence}
// 1st_order_sequence = 2nd_order_sequence {('+' | '-') 2nd_order_sequence}
// expression = 1st_order_sequence

bool parsing_read_expression(Reader* reader, Tree* output_tree);

