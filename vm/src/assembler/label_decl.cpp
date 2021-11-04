#include "assembler/label_decl.h"

#include "assembly/label.h"
#include "assembly/label_decl.h"

bool vm_assemble_label_decl(
	VmStatus* status,
	VmForwardStream* input_stream,
	VmForwardStream* output_stream,

	AssemblyLabels* labels,
	int32_t* ip
) {
	((void) output_stream);

	VmAssemblyLabel label = {};

	if (labels->num_labels >= VM_ASSEMBLY_MAX_NUMBER_OF_LABELS) {
		status->error = VM_ASSEMBLY_ERROR_TOO_MANY_LABELS;
		return false;
	}

	label->defined = true;
	label->addr = *ip;

	labels->labels[labels->num_labels] = label;
	labels->num_labels += 1;

	return true;
}
