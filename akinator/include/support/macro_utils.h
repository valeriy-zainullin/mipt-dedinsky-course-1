#pragma once

#include <stddef.h>
#include <stdio.h>

#if defined(__GNUC__)

#define MAY_BE_UNUSED __attribute__((unused))
#define UNREACHABLE assert(false); __builtin_unreachable()

#else

#define MAY_BE_UNUSED
#define UNREACHABLE

#endif

#define TO_STRING(MACRO) #MACRO
#define EXPANDED_TO_STRING(MACRO) TO_STRING(MACRO)
#define CONTAINER_OF(OBJECT, STRUCT, FIELD) offsetof

#define LOG_ERROR(...) {                                                                         \
	fprintf(stderr, __VA_ARGS__);                                                                \
	fprintf(stderr, " (file \"%s\", line %d, function \"%s\").\n", __FILE__, __LINE__, __func__); \
}
