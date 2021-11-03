#pragma once

struct VmAssemblyArgument {
	uint8_t arg_type;

	uint8_t register_index;

	VmAssemblyImmediateConst immediate_const;
};