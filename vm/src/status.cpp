#include "assembly/status.h"

#include "status.h"
#include "support/macro_utils.h"

#include <assert.h>

const char* vm_status_to_string(VmStatus status) {
	switch (status) {
		case VM_SUCCESS:                                     return "успешно";
		case VM_ASSEMBLY_ERROR_EMPTY_PROGRAM:                return "пустая программа";
		case VM_ASSEMBLY_ERROR_INVALID_LABEL_DECL:           return "неправильное объявление метки";
		case VM_ASSEMBLY_ERROR_INVALID_COMMAND:              return "неверная команда";
		case VM_ASSEMBLY_ERROR_INVALID_ARGUMENT:             return "неверный аргумент";
		case VM_ASSEMBLY_ERROR_INVALID_DIRECTIVE:            return "неверная директива";
		case VM_ASSEMBLY_ERROR_INVALID_EXPRESSION:           return "неверное выражение";
		case VM_ASSEMBLY_ERROR_LABEL_IS_TOO_LONG:            return "метка слишком длинная";
		case VM_ASSEMBLY_ERROR_TOO_MANY_LABELS:              return "слишком много меток";
		case VM_ASSEMBLY_ERROR_MULTIPLE_DEFINITION_OF_LABEL: return "повторное определение метки";
		case VM_ERROR_INSUFFICIENT_BUFFER:                   return "недостаточно буфера";
		case VM_ERROR_WHILE_WRITING:                         return "ошибка при записи";
		case VM_ERROR_WHILE_READING:                         return "ошибка при чтении";
		case VM_ERROR_DEBUGGER_NOT_PRESENT:                  return "отладчик отсутствует";
		case VM_STATUS_HALT_REQUESTED:                       return "запрошен выход";
		default: assert(false); UNREACHABLE;
	}
}