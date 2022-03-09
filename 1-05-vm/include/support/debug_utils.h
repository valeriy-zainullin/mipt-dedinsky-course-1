#pragma once

#include <assert.h>

#if VM_DEBUG && defined(NDEBUG)
#error "Asserts are not checked when debug is enabled."
#endif

#if VM_DEBUG
#define VERIFY(EXPR) assert(EXPR)
#else
#define VERIFY(EXPR) (void) (EXPR)
#endif