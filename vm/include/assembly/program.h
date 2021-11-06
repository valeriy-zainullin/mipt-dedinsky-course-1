#pragma once

#include "assembly/directive.h"
#include "assembly/operation.h"
#include "status.h"
#include "support/forward_stream.h"

#include "text.h"

bool vm_text_hook_on_program_start(VmStatus* status, void* argument);
bool vm_text_hook_on_program_end(VmStatus* status, void* argument);

bool vm_text_hook_on_directive(VmStatus* status, void* argument, VmAssemblyDirective* directive);
bool vm_text_hook_on_operation(VmStatus* status, void* argument, VmAssemblyOperation* operation);
bool vm_text_hook_on_label_decl(VmStatus* status, void* argument, char* label);

bool vm_text_process_directive(VmStatus* status, VmForwardStream* stream, void* argument);
bool vm_text_process_operation(VmStatus* status, VmForwardStream* stream, void* argument);
bool vm_text_process_label_decl(VmStatus* status, VmForwardStream* stream, void* argument);
bool vm_text_process_line(VmStatus* status, unsigned char* line, size_t length, void* argument);

VmAssemblyStatus vm_text_process_program(TextLines* lines, void* argument);
