#pragma once

#include "tree/tree.h"

#include <stdio.h>

#define LOG_ERROR_FOR_NODE(NODE, ...) fprintf(stderr, "Node@%p: ", (void*) NODE); LOG_ERROR(__VA_ARGS__)

