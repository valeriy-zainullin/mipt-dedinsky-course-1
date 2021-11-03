#pragma once

#include "macro_utils.h"
#include "status.h"

#include <stdbool.h>
#include <stddef.h>

#define VM_LABEL_NAME_MAX_LENGTH_MACRO 256
#define VM_LABEL_NAME_SCANF_FORMAT " %" EXPAND(VM_LABEL_NAME_MAX_LENGTH_MACRO) "[a-zA-Z0-9_]"

static const size_t VM_ASSEMBLY_MAX_NUMBER_OF_LABELS = 1024;

static const size_t VM_ASSEMBLY_MAX_LABEL_LENGTH = VM_LABEL_NAME_MAX_LENGTH_MACRO;
