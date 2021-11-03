#include "assembly/argument.h"
#include "assembly/command.h"
#include "assembly/status.h"

#include <stdbool.h>

struct VmAssemblyOperation {
	char command[VM_COMMAND_NAME_MAX_LENGTH + 1];
	VmAssemblyArgument argument;
};

bool vm_text_read_operation(VmAssemblyStatus* status, FILE* input_stream, VmAssemblyOperation* operation);
bool vm_text_write_operation(VmAssemblyStatus* status, FILE* output_stream, const VmAssemblyOperation* operation);
