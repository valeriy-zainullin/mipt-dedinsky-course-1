#pragma once

#include <inttypes.h>

#if defined(QUADRATIC_EQUATION_IMPL)
/*static*/ const double COMPARISON_EPS = 1e-5L;
#endif

void process_output(int8_t number_of_roots, double* roots);

int8_t solve_linear(double parameter_b, double parameter_c, double* roots);

int8_t solve_quadratic(
	double parameter_a,
	double parameter_b,
	double parameter_c,
	double* roots
);

void solve(double parameter_a, double parameter_b, double parameter_c);
