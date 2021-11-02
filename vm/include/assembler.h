#pragma once

#include "status.h"

#include "text.h"

#include <stdio.h>

VmAssemblyStatus vm_assemble(TextLines* lines, FILE* output_stream);
