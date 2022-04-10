#include <stddef.h>

typedef void* HANDLE;
typedef void* LPVOID;
typedef unsigned int DWORD;
typedef size_t SIZE_T;
typedef int BOOL;

__declspec(dllexport)
LPVOID
HeapAlloc(
	HANDLE hHeap,
	DWORD  dwFlags,
	SIZE_T dwBytes
) {
	(void) hHeap;
	(void) dwFlags;
	(void) dwBytes;
	
	return NULL;
}

__declspec(dllexport)
BOOL
HeapFree(
	HANDLE hHeap,
	DWORD  dwFlags,
	LPVOID lpMem
) {
	(void) hHeap;
	(void) dwFlags;
	(void) lpMem;
	
	return 0;
}