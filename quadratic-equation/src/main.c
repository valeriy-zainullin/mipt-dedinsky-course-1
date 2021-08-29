#include "quadratic-equation.h"

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void print_usage(const char* program_name) {
	printf(
		"Usage: %s a b c\n"
		"Solves quadratic equation (ax^2 + bx + c = 0, \"a\" can be 0). "
		"Prints roots.\n",
		program_name
	);
}

static bool parse_double(const char* string, double* result_ptr) {
	if (!isdigit(string[0]) && string[0] != '-' && string[0] != '+') {
		return false;
	}

	char after_double = 0;
	int scanf_return_value = sscanf(string, "%lf%c", result_ptr, &after_double);
	return scanf_return_value == 1;
}

void process_output(int8_t number_of_roots, double* roots) {
	switch(number_of_roots) {
		case -1: printf("ANY_NUMBER\n"); break;
		case 0: printf("NO ROOTS\n"); break;
		case 1: printf("%f\n", roots[0]); break;
		case 2: printf("%f %f\n", roots[0], roots[1]); break;
		default: assert(false);
	}
}

static bool parse_arguments(
	int argc,
	char** argv,
	double* ptr_to_parameter_a,
	double* ptr_to_parameter_b,
	double* ptr_to_parameter_c
) {
	if (argc != 4) {
		print_usage(argv[0]);
		printf("Wrong number of arguments.\n");
		return false;
	}

	const char* parameter_a_as_string = argv[1];
	const char* parameter_b_as_string = argv[2];
	const char* parameter_c_as_string = argv[3];

	if (
		!parse_double(parameter_a_as_string, ptr_to_parameter_a) ||
		!parse_double(parameter_b_as_string, ptr_to_parameter_b) ||
		!parse_double(parameter_c_as_string, ptr_to_parameter_c)
	) {
		print_usage(argv[0]);
		printf("One of the arguments is not a 32-bit signed integer.\n");
		return false;
	}

	return true;
}

static const int SUCCESS = 0;
static const int WRONG_USAGE = 1;

int main(int argc, char** argv) {
	double parameter_a = 0;
	double parameter_b = 0;
	double parameter_c = 0;
	if (!parse_arguments(argc, argv, &parameter_a, &parameter_b, &parameter_c)) {
		return WRONG_USAGE;
	}

	solve(parameter_a, parameter_b, parameter_c);
	return SUCCESS;
}
