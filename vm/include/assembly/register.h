#pragma once

// 3 registers: ax, bx, cx.

#include <stddef.h>

static const size_t VM_MAX_REGISTER_NAME_INDEX = 2;

typedef VmRegisterType uint32_t;

#define VM_REGISTER_NAME_SCANF_FORMAT " %1[a-c]x"
