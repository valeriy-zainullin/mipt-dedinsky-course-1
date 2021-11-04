#pragma once

#include "../status.h" // TODO: fix.

#include <stddef.h>

struct VmAssemblyStatus {
	size_t line;
	VmStatus error;
};