#pragma once

struct SourceLocation {
	const char* file_name;
	size_t line;
	const char* function;
};
typedef struct SourceLocation SourceLocation;
#define LOCATION SourceLocation{__FILE__, __LINE__, __func__}
