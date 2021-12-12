#pragma once

#include "reader.h"
#include "tree.h"

#include <stdbool.h>

// number = '0' | ('1'..'9' {'0'..'9'})
// expression_in_braces = '(' expression ')'
// value = ['+' | '-'] (number | function | expression_in_braces)
// 3rd_order_sequence = value ['^' value]
// 2nd_order_sequence = 3rd_order_sequence {('*' | '/') 3rd_order_sequence}
// 1st_order_sequence = 2nd_order_sequence {('+' | '-') 2nd_order_sequence}
// expression = 1st_order_sequence

bool parsing_read_expression(Reader* reader, Tree* output_tree);

