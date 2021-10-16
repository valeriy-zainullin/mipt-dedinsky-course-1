#include "quadratic-equation.h"

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
	char* end_ptr;
	*result_ptr = strtod(string, &end_ptr);
	return end_ptr == string + strlen(string);
}

void process_output(int8_t number_of_roots, double* roots) {
	switch (number_of_roots) {
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
