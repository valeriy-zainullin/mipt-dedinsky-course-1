#include "assembly/label.h"
#include "assembly/program.h"
#include "assembly/status.h"

#include <assert.h>
#include <stdint.h>

#if defined(__GNUC__)
#define UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE __assume(0)
#else
#define UNREACHABLE
#endif

struct VmAssemblyLabel {
	bool defined;
	char name[VM_ASSEMBLY_MAX_LABEL_LENGTH + 1];
	size_t addr = 0;
};

struct VmAssemblyLabels {
	VmAssemblyLabel labels[VM_ASSEMBLY_MAX_NUMBER_OF_LABELS];
	size_t nlabels;
};

struct Assembler {

	FILE* output_file;

	int32_t ip;
	VmAssemblyLabels labels;

};

bool vm_text_hook_on_program_start(VmAssemblyStatus* status, void* argument) {
	assert(status != nullptr);
	assert(argument != nullptr);

	Assembler* assembler = (Assembler*) argument;

	assembler->ip = 0;
	assembler->labels.nlabels = 0;

	return true;

}

bool vm_text_hook_on_program_end(VmAssemblyStatus* status, void* argument) {
	assert(status != nullptr);
	assert(argument != nullptr);

	Assembler* assembler = (Assembler*) argument;

	if (assembler->ip == 0) {
		*status = VM_ASSEMBLY_ERROR_EMPTY_PROGRAM;
		return false;
	}

	return true;	

}

bool vm_text_hook_on_operation(VmAssemblyStatus* status, void* argument, VmAssemblyOperation* operation) {
	assert(status != nullptr);
	assert(argument != nullptr);
	assert(operation != nullptr);

	Assembler* assembler = (Assembler*) argument;

	VmOperation vm_operation = {};

	#define COMMAND(NAME, INDEX, ALLOWED_ARG_TYPES, ...) \
		if (strcmp(operation->command, #NAME) == 0) {    \
			vm_operation.command_index = INDEX;          \
		} else
	#include "commands.h"
	#undef COMMAND
	{
		// The command validity was checked on reading stage.
		assert(false);
		UNREACHABLE;
	}

	vm_operation.arg_type = operation.arg_type;
	vm_operation.register_index = operation.register_index;

	return vm_text_write_

}

bool vm_text_hook_on_label_decl(VmAssemblyStatus* status, void* argument, char* label);
