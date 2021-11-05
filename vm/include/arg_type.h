#pragma once

#include "support/macro_utils.h"

#include <stdint.h>

MAY_BE_UNUSED static const uint8_t VM_COMMAND_ARG_NOT_PRESENT = 0;
MAY_BE_UNUSED static const uint8_t VM_COMMAND_ARG_USES_MEMORY = (1 << 2);
MAY_BE_UNUSED static const uint8_t VM_COMMAND_ARG_USES_REGISTER = (1 << 1);
MAY_BE_UNUSED static const uint8_t VM_COMMAND_ARG_USES_IMMEDIATE_CONST = 1;

/*
union VmArgType {
	uint8_t arg_type;

	// implementation-defined order of fields.
	struct VmArgTypeStruct {
		bool uses_register : 1;
		bool uses_memory : 1;
		bool uses_immediate_const : 1;
	};
};
*/
