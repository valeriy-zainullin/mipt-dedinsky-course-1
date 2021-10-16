#define QUADRATIC_EQUATION_IMPL
#include "quadratic-equation.h"

#include <inttypes.h>
#include <math.h>

static double are_equal(double lhs, double rhs) {
	return fabs(lhs - rhs) < COMPARISON_EPS;
}

int8_t solve_linear(double parameter_b, double parameter_c, double* roots) {
	if (are_equal(parameter_b, 0)) {
		if (are_equal(parameter_c, 0)) {
			return -1;
		}
		return 0;
	}

	roots[0] = parameter_c / parameter_b;
	return 1;
}

int8_t solve_quadratic(
	double parameter_a,
	double parameter_b,
	double parameter_c,
	double* roots
) {
	// D = b^2 - 4 * a * c.
	// x1 = (-b - sqrt(D)) / (2 * a), x2 = (-b + sqrt(D)) / (2 * a).
	// vertice = -b / (2 * a).
	// distance = sqrt(D) / (2 * a).
	double discriminant =
		parameter_b * parameter_b - 4 * parameter_a * parameter_c;

	if (are_equal(discriminant, 0)) {
		double vertice = -parameter_b / (2 * parameter_a);
		roots[0] = vertice;
		return 1;
	} else if (discriminant < 0) {
		return 0;
	}

	double distance = sqrt(discriminant) / (2 * parameter_a);
	double vertice = -parameter_b / (2 * parameter_a);

	roots[0] = vertice - distance;
	roots[1] = vertice + distance;
	return 2;
}

void solve(double parameter_a, double parameter_b, double parameter_c) {
	int8_t number_of_roots = 0;
	double roots[2] = {0, 0};
	if (are_equal(parameter_a, 0)) {
		number_of_roots = solve_linear(parameter_b, parameter_c, roots);
	} else {
		number_of_roots =
			solve_quadratic(parameter_a, parameter_b, parameter_c, roots);
	}
	process_output(number_of_roots, roots);
}
