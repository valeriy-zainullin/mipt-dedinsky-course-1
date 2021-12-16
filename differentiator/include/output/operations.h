#pragma once

#include "macroutils.h"

#include <assert.h>

static const int OPERATION_ORDER_MAX = 3;

static int operation_get_operation_order(char operation) {
	switch (operation) {
		case '^': {
			return 1;
		}
		
		case '*':
		case '/': {
			return 2;
		}
		
		case '+':
		case '-': {
			return 3;
		}
		
		default: assert(false); UNREACHABLE;
	}
}
