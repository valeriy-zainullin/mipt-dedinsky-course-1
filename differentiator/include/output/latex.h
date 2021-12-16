#pragma once

#include "tree.h"

#include <stddef.h>
#include <stdio.h>

struct LatexWriter {
	FILE* output_stream;
	bool is_debug;
};

void latex_writer_init(LatexWriter* latex_writer, FILE* output_stream, bool is_debug);
void latex_writer_deinit(LatexWriter* latex_writer);

void latex_writer_before_differentiation(LatexWriter* latex_writer);
void latex_writer_on_differentiation_started(LatexWriter* latex_writer);
void latex_writer_on_differentiation_ended(LatexWriter* latex_writer);
void latex_writer_after_differentiation(LatexWriter* latex_writer);

// bool latex_write_tree(FILE* output_stream, Tree* tree);
