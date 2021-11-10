#pragma once

#include "../status.h" // TODO: fix.

#include <stddef.h>

struct VMAssemblyStatus {
	size_t line;
	VMStatus error;
};