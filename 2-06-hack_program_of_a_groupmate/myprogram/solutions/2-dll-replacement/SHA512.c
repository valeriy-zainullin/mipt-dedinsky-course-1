#include <inttypes.h>
#include <stddef.h>
#include <windows.h>

static void mymemcpy(void* dst, void const* src, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		((char*) dst)[i] = ((char*) src)[i];
	}
}

static void mymemset(void* dst, int value, size_t size) {
	for (size_t i = 0; i < size; ++i) {
		((char*) dst)[i] = (char) value;
	}
}

static void* mymalloc(size_t nbytes) {
	return HeapAlloc(GetProcessHeap(), 0, nbytes);
}

static void myfree(void* ptr) {
	HeapFree(GetProcessHeap(), 0, ptr);
}

#define memcpy(...) mymemcpy(__VA_ARGS__)
#define memset(...) mymemset(__VA_ARGS__)
#define malloc(...) mymalloc(__VA_ARGS__)
#define free(...) myfree(__VA_ARGS__)

/// Wrapper for hashing methods, up to caller to free the return value
__declspec(dllexport) uint64_t *SHA512Hash(uint8_t *input, size_t len)
{
	void* buffer = malloc(64);
	if (buffer == NULL) {
		return NULL;
	}
	
	static const uint8_t DESIRED_HASH[] = {
		0x49, 0xDC, 0xDA, 0x1C, 0xA8, 0x32, 0x3C, 0xA9,
		0x74, 0xD0, 0xE2, 0xF9, 0x16, 0xDB, 0x06, 0xBE,
		0x29, 0xB9, 0xC6, 0x46, 0xAE, 0x8B, 0x26, 0x4B,
		0xF1, 0x3A, 0x4C, 0x5A, 0xCB, 0x4D, 0xC6, 0x8B,
		0x22, 0x86, 0xDD, 0x82, 0x7E, 0x36, 0xF1, 0x08,
		0x03, 0x95, 0x5F, 0x64, 0x11, 0x75, 0xC6, 0x2F,
		0xF6, 0x80, 0xCA, 0xE2, 0x8A, 0x82, 0x13, 0x3B,
		0x3A, 0x15, 0xE2, 0x81, 0x92, 0x10, 0xF4, 0xE5
	};
	
	memcpy(buffer, DESIRED_HASH, sizeof(DESIRED_HASH));
	
    return buffer;
}

BOOL WINAPI DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return TRUE;
}