#pragma once

#define ARRAY_DEF(...) { __VA_ARGS__ }

// To str?
#define EXPAND2(MACRO) #MACRO
#define EXPAND(MACRO) EXPAND2(MACRO)

#define MAY_BE_UNUSED __attribute__((unused))

#if defined(__GNUC__)
#define UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE __assume(0)
#else
#define UNREACHABLE
#endif
