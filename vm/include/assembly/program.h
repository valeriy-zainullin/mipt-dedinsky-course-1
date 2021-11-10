#pragma once

#include "assembly/directive.h"
#include "assembly/operation.h"
#include "status.h"
#include "support/forward_stream.h"

#include "text.h"

bool vm_text_hook_on_program_start(VMStatus* status, void* argument);
bool vm_text_hook_on_program_end(VMStatus* status, void* argument);

bool vm_text_hook_on_directive(VMStatus* status, void* argument, VMAssemblyDirective* directive);
bool vm_text_hook_on_operation(VMStatus* status, void* argument, VMAssemblyOperation* operation);
bool vm_text_hook_on_label_decl(VMStatus* status, void* argument, char* label);

bool vm_text_process_directive(VMStatus* status, VMForwardStream* stream, void* argument);
bool vm_text_process_operation(VMStatus* status, VMForwardStream* stream, void* argument);
bool vm_text_process_label_decl(VMStatus* status, VMForwardStream* stream, void* argument);
bool vm_text_process_line(VMStatus* status, unsigned char* line, size_t length, void* argument);

VMAssemblyStatus vm_text_process_program(TextLines* lines, void* argument);
