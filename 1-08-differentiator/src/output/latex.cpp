#include "output/latex.h"

#include "macroutils.h"
#include "stringutils.h"
#include "tree.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

bool latex_writer_init(LatexWriter* latex_writer, const char* output_file, bool is_debug) {
	latex_writer->output_file = output_file;
	latex_writer->tmp_file = NULL;
	if (!string_concat(&latex_writer->tmp_file, latex_writer->output_file, ".tex")) {
		return false;
	}
	
	latex_writer->stream = fopen(latex_writer->tmp_file, "w");
	if (latex_writer->stream == NULL) {
		string_free(&latex_writer->tmp_file);
		return false;
	}
	
	latex_writer->is_debug = is_debug;
	
	return true;
}

void latex_writer_deinit(LatexWriter* latex_writer) {
	if (latex_writer->stream != NULL) {
		fclose(latex_writer->stream);
	}
	string_free(&latex_writer->tmp_file);
}

void latex_writer_before_differentiation(void* callback_arg, const Tree* tree, Tree* simplified_tree) {
	LatexWriter* latex_writer = (LatexWriter*) callback_arg;
	
	fprintf(latex_writer->stream, "\\documentclass{article}\n");
	fprintf(latex_writer->stream, "\\usepackage[T2A]{fontenc}\n");
	fprintf(latex_writer->stream, "\\usepackage[utf8]{inputenc}\n");
	fprintf(latex_writer->stream, "\\usepackage[russian]{babel}\n");
	fprintf(latex_writer->stream, "\\usepackage{amssymb}\n");
	fprintf(latex_writer->stream, "\\usepackage{amsmath}\n");
	fprintf(latex_writer->stream, "\\begin{document}\n");
	
	fprintf(latex_writer->stream, "Здравствуйте. Вы мне дали это: $$");
	latex_writer_print_expression(latex_writer, tree->root);
	fprintf(latex_writer->stream, "$$ и сказали продифференцировать. Ну что ж, получайте.\n\n");
	
	fprintf(latex_writer->stream, "В свою очередь я упрощаю то, что вы мне дали. Думаю теперь про такой ввод: $$");
	latex_writer_print_expression(latex_writer, simplified_tree->root);
	fprintf(latex_writer->stream, ".$$\n\n");

	latex_writer->step = 1;
}

void latex_writer_on_differentiation_started(void* callback_arg, const TreeNode* node) {
	(void) callback_arg;
	(void) node;
}

void latex_writer_on_differentiation_ended(void* callback_arg, const TreeNode* node, TreeNode** output_node) {
	LatexWriter* latex_writer = (LatexWriter*) callback_arg;
	
	fprintf(latex_writer->stream, "\\textbf{Шаг \\#%d.} $${\\left( ", latex_writer->step);
	latex_writer_print_expression(latex_writer, node);
	fprintf(latex_writer->stream, " \\right) }' = ");
	latex_writer_print_expression(latex_writer, *output_node);
	fprintf(latex_writer->stream, ".$$\n\n");
	
	latex_writer->step += 1;
}

static const size_t MAX_COMMAND_LENGTH = 256;
void latex_writer_after_differentiation(void* callback_arg, const Tree* tree) {
	LatexWriter* latex_writer = (LatexWriter*) callback_arg;
	
	fprintf(latex_writer->stream, "Производная у нас получилась такая: $$\n");
	latex_writer_print_expression(latex_writer, tree->root);
	fprintf(latex_writer->stream, ".$$ Вам удачи и пока.\n");
	
	fprintf(latex_writer->stream, "\\end{document}\n");
	
	fclose(latex_writer->stream);
	latex_writer->stream = NULL;
	
	char command_buffer[MAX_COMMAND_LENGTH + 1] = {};
	snprintf(command_buffer, MAX_COMMAND_LENGTH, "pdflatex --jobname \"%s\" -interaction=nonstopmode -halt-on-error \"%s\"", latex_writer->output_file, latex_writer->tmp_file);
	system(command_buffer);
}

void latex_writer_print_expression(LatexWriter* latex_writer, const TreeNode* node) {
	assert(node != NULL);
	
	switch (node->type) {
		case TREE_NODE_TYPE_OPERATION: {
			switch (node->operation) {
				case '+':
				case '-': {
					latex_writer_print_expression(latex_writer, node->lhs);
					fprintf(latex_writer->stream, " %c ", node->operation);
					latex_writer_print_expression(latex_writer, node->rhs);
					
					break;
				}
				
				case '*': {
					latex_writer_print_expression(latex_writer, node->lhs);
					fprintf(latex_writer->stream, " \\cdot ");
					latex_writer_print_expression(latex_writer, node->rhs);
					
					break;
				}
				
				case '/': {
					fprintf(latex_writer->stream, "\\frac{");
					latex_writer_print_expression(latex_writer, node->lhs);
					fprintf(latex_writer->stream, "}{");
					latex_writer_print_expression(latex_writer, node->rhs);
					fprintf(latex_writer->stream, "}");
					
					break;
				}
				
				case '^': {
					fprintf(latex_writer->stream, "{\\left(");
					latex_writer_print_expression(latex_writer, node->lhs);
					fprintf(latex_writer->stream, "\\right)}^{");
					latex_writer_print_expression(latex_writer, node->rhs);
					fprintf(latex_writer->stream, "}");
					
					break;
				}
				
				default: assert(false); UNREACHABLE;
			}
			
			break;
		}
		
		case TREE_NODE_TYPE_FUNCTION: {
			fprintf(latex_writer->stream, "\\%s\\left(", node->name);
			latex_writer_print_expression(latex_writer, node->inner);
			fprintf(latex_writer->stream, "\\right)");
			
			break;
		}
		
		case TREE_NODE_TYPE_VARIABLE: {
			fprintf(latex_writer->stream, "%s", node->name);
			
			break;
		}
		
		case TREE_NODE_TYPE_NUMBER: {
			fprintf(latex_writer->stream, "%d", node->number);
			
			break;
		}
		
		default: assert(false); UNREACHABLE;
	}
}

