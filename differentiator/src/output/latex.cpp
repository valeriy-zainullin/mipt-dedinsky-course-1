#include "output/latex.h"

#include "tree.h"

#include <stdio.h>

void latex_writer_init(LatexWriter* latex_writer, FILE* output_stream, bool is_debug) {
	latex_writer->output_stream = output_stream;
	latex_writer->is_debug = is_debug;
}

void latex_writer_deinit(LatexWriter* latex_writer) {
	(void) latex_writer;
}

void latex_writer_before_differentiation(LatexWriter* latex_writer) {
	(void) latex_writer;
}

void latex_writer_on_differentiation_started(LatexWriter* latex_writer) {
	(void) latex_writer;
}

void latex_writer_on_differentiation_ended(LatexWriter* latex_writer) {
	(void) latex_writer;
}

void latex_writer_after_differentiation(LatexWriter* latex_writer) {
	(void) latex_writer;
}

