#pragma once

#include "tree/tree.h"

bool database_read(Tree* tree, FILE* input_stream);
bool database_save(Tree* tree, FILE* output_stream);
