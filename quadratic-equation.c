#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <stddef.h>
#include <limits.h>

static void print_usage(const char* program_name) {
	printf(
		"Usage: %s a b c\n"
		"Solves quadratic equation (ax^2 + bx + c = 0, where a != 0). Prints roots.\n",
		program_name
	);
}

static const size_t INT32_MAX_LENGTH = 10;

static bool parse_int32(const char* string, int32_t* result_ptr) {
	if (string[0] < '0' || string[0] > '9') {
		return false;
	}

	size_t string_length = strlen(string);
	// Protect from a too long string for "%n".
	if (string_length > INT32_MAX_LENGTH) {
		return false;
	}

	int num_of_characters_read;
	int scanf_return_value = sscanf(string, "%" SCNd32 "%n", result_ptr, &num_of_characters_read);
	return scanf_return_value == 1 && ((size_t) num_of_characters_read) == string_length;
}

static const int WRONG_USAGE = 1;
static const int SUCCESS = 0;

int main(int argc, char** argv) {
	if (argc != 4) {
		print_usage(argv[0]);
		printf("Wrong number of arguments.\n");
		return WRONG_USAGE;
	}

	const char* parameter_a_as_string = argv[1];
	const char* parameter_b_as_string = argv[2];
	const char* parameter_c_as_string = argv[3];

	int32_t parameter_a;
	int32_t parameter_b;
	int32_t parameter_c;

	if (
		!parse_int32(parameter_a_as_string, &parameter_a) ||
		!parse_int32(parameter_b_as_string, &parameter_b) ||
		!parse_int32(parameter_c_as_string, &parameter_c)
	) {
		print_usage(argv[0]);
		printf("One of the arguments is not a 32-bit signed integer.\n");
		return WRONG_USAGE;
	}

	if (parameter_a == 0) {
		print_usage(argv[0]);
		printf("Not a quadratic equation (a = 0).\n");
		return WRONG_USAGE;
	}

	// D = b^2 - 4 * a * c.
	// [-2^31, 2^31 - 1] * [-2^31, 2^31 - 1] = [-2^62 + 2^31, 2^62].
	int64_t determinant =
		((int64_t) parameter_b) * parameter_b -
		((int64_t) 4) * parameter_a * parameter_c;

	if (determinant < 0) {
		return SUCCESS;
	}

	// x1 = (-b - sqrt(D)) / (2 * a), x2 = (-b + sqrt(D)) / (2 * a).
	// vertice = -b / (2 * a).
	// distance = sqrt(D) / (2 * a).
	double vertice = -parameter_b / (2 * parameter_a);
	if (determinant == 0) {
		printf("%f\n", vertice);
		return SUCCESS;
	}

	double distance = sqrt(determinant) / (2 * parameter_a);
	double x1 = vertice - distance;
	double x2 = vertice + distance;
	printf("%f %f\n", x1, x2);

	return SUCCESS;
}