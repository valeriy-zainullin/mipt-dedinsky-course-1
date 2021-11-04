#pragma once

#include "../status.h" // TODO.
#include "assembly/label.h"
#include "support/forward_stream.h"

#include <stdbool.h>
#include <stdio.h>

#define VM_ASSEMBLY_LABEL_DECL_SCANF_FORMAT VM_ASSEMBLY_LABEL_NAME_SCANF_FORMAT ":"

bool vm_text_lookahead_line_is_label_decl(unsigned char* line, size_t length);

bool vm_text_read_label_decl(VmStatus* status, VmForwardStream* input_stream, char* label);
