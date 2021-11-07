#include "cli.h"
#include "support/debug_utils.h"
#include "vm/state.h"
#include "vm/vm.h"

#include "text.h"

#include <stdio.h>


bool vm_on_trap_to_debugger(VmStatus* status, VmState* state, bool* continue_execution, void* debugger_arg) {
	assert(status != NULL);
	assert(state != NULL);
	assert(continue_execution != NULL);

	((void) status);
	((void) state);
	((void) continue_execution);
	((void) debugger_arg);

	*status = VM_ERROR_DEBUGGER_NOT_PRESENT;

	return false;
}

bool vm_on_debugger_notified(VmStatus* status, VmState* state, bool* continue_execution, void* debugger_arg) {
	assert(status != NULL);
	assert(state != NULL);
	assert(continue_execution != NULL);

	((void) status);
	((void) state);
	((void) continue_execution);
	((void) debugger_arg);

	return true;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		return 1;
	}
	const char* input_file = argv[1];

	FILE* input_stream = fopen(input_file, "rb");
	if (input_stream == NULL) {
		return 4;
	}

	vm_execute(input_stream, stdin, stdout, NULL);

	// return (int) CLI_EXIT_CODE_SUCCESS;
	return 0;
}