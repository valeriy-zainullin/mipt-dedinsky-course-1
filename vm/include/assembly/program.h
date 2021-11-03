#pragma once

#include "assembly/operation.h"
#include "assembly/status.h"

bool vm_text_hook_on_program_start(VmAssemblyStatus* status, void* argument);
bool vm_text_hook_on_program_end(VmAssemblyStatus* status, void* argument);

bool vm_text_hook_on_operation(VmAssemblyStatus* status, void* argument, VmAssemblyOperation* operation);
bool vm_text_hook_on_label_decl(VmAssemblyStatus* status, void* argument, char* label);

bool vm_text_process_operation(VmAssemblyStatus* status, VmForwardStream* stream, void* argument);
bool vm_text_process_label_decl(VmAssemblyStatus* status, VmForwardStream* stream, void* argument);
bool vm_text_process_line(VmAssemblyStatus* status, unsigned char* line, size_t length, void* argument);

VmAssemblyStatus vm_text_process_program(TextLines* lines, void* argument);
