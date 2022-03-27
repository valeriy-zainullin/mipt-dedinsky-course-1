#pragma once

#include <stddef.h>
#include <stdio.h>

#if defined(__GNUC__)

#define MAY_BE_UNUSED __attribute__((unused))

#if !defined(UNREACHABLE)
#define UNREACHABLE assert(false); __builtin_unreachable()
#endif

#else

#define MAY_BE_UNUSED

#if !defined(UNREACHABLE)
#define UNREACHABLE
#endif

#endif

#define TO_STRING(MACRO) #MACRO
#define EXPANDED_TO_STRING(MACRO) TO_STRING(MACRO)
