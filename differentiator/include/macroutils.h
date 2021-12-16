#pragma once

#if defined(__GNUC__) || defined(__clang__)
#define UNREACHABLE __builtin_unreachable()
#define EXPECT(a, b) __builtin_expect(a, b)
#else
#define UNREACHABLE
#define EXPECT(a, b) a
#endif

