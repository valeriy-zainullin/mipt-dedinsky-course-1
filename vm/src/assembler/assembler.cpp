#include "assembler/assembler.h"

#include "assembly/directive.h"
#include "assembly/label.h"
#include "assembly/program.h"
#include "bytecode/operation.h"
#include "status.h"
#include "support/macro_utils.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

bool vm_text_hook_on_program_start(VmStatus* status, void* argument) {
	assert(status != NULL);
	assert(argument != NULL);

	VmAssembler* assembler = (VmAssembler*) argument;

	assembler->ip = 0;

	return true;

}

bool vm_text_hook_on_program_end(VmStatus* status, void* argument) {
	assert(status != NULL);
	assert(argument != NULL);

	VmAssembler* assembler = (VmAssembler*) argument;

	if (assembler->ip == 0) {
		*status = VM_ASSEMBLY_ERROR_EMPTY_PROGRAM;
		return false;
	}

	return true;	

}

bool vm_text_hook_on_directive(VmStatus* status, void* argument, VmAssemblyDirective* directive) {
	assert(status != NULL);
	assert(argument != NULL);
	assert(directive != NULL);

	VmAssembler* assembler = (VmAssembler*) argument;

	((void) assembler);
	((void) directive);

	if (strcmp(directive->name, "db") == 0) {
		for (size_t i = 0; i < directive->num_arguments; ++i) {
			VmAssemblyDirectiveArgument* directive_argument = &directive->arguments[i];
			switch (directive_argument->argument_type) {
				case VM_ASSEMBLY_DIRECTIVE_ARG_STRING:
					if (
						fwrite(
							directive_argument->string,
							sizeof(*directive_argument->string),
							directive_argument->string_length,
							assembler->output_file
						) !=
						directive_argument->string_length
					) {
						*status = VM_ERROR_WHILE_WRITING;
						return false;
					}
					assert(directive_argument->string_length <= INT32_MAX);
					assembler->ip += (int32_t) directive_argument->string_length;
					break;
				case VM_ASSEMBLY_DIRECTIVE_ARG_NUMBER:
					if (fwrite(&directive_argument->number, sizeof(directive_argument->number), 1, assembler->output_file) != 1) {
						*status = VM_ERROR_WHILE_WRITING;
						return false;
					}
					assembler->ip += sizeof(directive_argument->number);
					break;
				default:
					assert(false);
					UNREACHABLE;
			}
		}
	} else {
		*status = VM_ASSEMBLY_ERROR_INVALID_DIRECTIVE;
		return false;
	}

	return true;
}

bool vm_text_hook_on_operation(VmStatus* status, void* argument, VmAssemblyOperation* operation) {
	assert(status != NULL);
	assert(argument != NULL);
	assert(operation != NULL);

	VmAssembler* assembler = (VmAssembler*) argument;

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

	vm_operation.arg_type = operation->argument.arg_type;
	vm_operation.register_index = operation->argument.register_index;

	if (operation->argument.immediate_const.is_label) {
		VmAssemblyLabel* label = NULL;
		for (size_t i = 0; i < assembler->labels.nlabels; ++i) {
			if (strcmp(assembler->labels.labels[i].name, operation->argument.immediate_const.label) == 0) {
				label = &assembler->labels.labels[i];
				break;
			}
		}

		if (label == NULL) {
			if (assembler->labels.nlabels >= VM_ASSEMBLY_MAX_NUMBER_OF_LABELS) {
				*status = VM_ASSEMBLY_ERROR_TOO_MANY_LABELS;
				return status;
			}
			label = &assembler->labels.labels[assembler->labels.nlabels];
			assembler->labels.nlabels += 1;

			strcpy(assembler->labels.labels[assembler->labels.nlabels].name, operation->argument.immediate_const.label);
			assembler->labels.labels[assembler->labels.nlabels].defined = false;
		}

		if (label->defined) {
			vm_operation.immediate_const = label->addr;
		} else {
			vm_operation.immediate_const = 0;
		}
	} else {
		vm_operation.immediate_const = operation->argument.immediate_const.value;
	}

	unsigned char output[VM_BYTECODE_MAX_OPERATION_LENGTH];
	VmForwardStream output_stream = {};
	output_stream.bytes = output;
	output_stream.offset = 0;
	output_stream.length = VM_BYTECODE_MAX_OPERATION_LENGTH;

	VmStatus vm_status;
	if (!vm_bytecode_write_operation(&vm_status, &output_stream, &vm_operation)) {
		return false;
	}

	if (fwrite(output, sizeof(unsigned char), output_stream.offset, assembler->output_file) != output_stream.offset) {
		*status = VM_ERROR_WHILE_WRITING;
		return false;
	}

	assert(output_stream.offset <= INT32_MAX);
	assembler->ip += (int32_t) output_stream.offset;

	return true;

}

bool vm_text_hook_on_label_decl(VmStatus* status, void* argument, char* label_name) {
	assert(status != NULL);
	assert(argument != NULL);
	assert(label_name != NULL);

	VmAssembler* assembler = (VmAssembler*) argument;
	
	VmAssemblyLabel* label = NULL;

	for (size_t i = 0; i < assembler->labels.nlabels; ++i) {
		if (strcmp(assembler->labels.labels[i].name, label_name) == 0) {
			if (assembler->pass == 1 && assembler->labels.labels[i].defined) {
				*status = VM_ASSEMBLY_ERROR_MULTIPLE_DEFINITION_OF_LABEL;
				return false;
			}

			label = &assembler->labels.labels[i];
			break;
		}
	}

	if (label == NULL) {
		fflush(stdout);
		assert(assembler->pass == 1);
	
		if (assembler->labels.nlabels >= VM_ASSEMBLY_MAX_NUMBER_OF_LABELS) {
			// Don't do a second pass then.
			*status = VM_ASSEMBLY_ERROR_TOO_MANY_LABELS;
			return false;
		}

		label = &assembler->labels.labels[assembler->labels.nlabels];
		assembler->labels.nlabels += 1;

		strcpy(label->name, label_name);
	}

	label->defined = true;
	label->addr = assembler->ip;

	return true;

}
