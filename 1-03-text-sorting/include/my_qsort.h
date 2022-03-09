#pragma once

#include <stddef.h>

void my_qsort(void* items, size_t length, size_t item_size, int(*compare)(const void*, const void*));
