#include <limits.h>

#if defined(__cplusplus)
extern "C" {
#endif

__cdecl int myprintf(const char* fmt, ...);

#if defined(__cplusplus)
}
#endif

int main() {
	myprintf(
		"%%%% = %%\n"
		"%%d(%s) = %d\n"
		"%%d(%s) = %d\n"
		"%%d(%s) = %d\n"
		"%%ld(%s) = %ld\n"
		"%%ld(%s) = %ld\n"
		"%%ld(%s) = %ld\n"
		"%%ld(%s) = %ld\n"
		"%%lld(%s) = %lld\n"
		"%%lld(%s) = %lld\n"
		"%%lld(%s) = %lld\n"
		"%%lld(%s) = %lld\n"
		"%%b(%s) = %b\n"
		"%%s(%s) = %s\n"
		"%%c(%s) = %c\n"
		// "%%c(%s) = %c\n" // TODO: print negative values.
		"%%x(%s) = %x\n"
		"%%llx(%s) = %llx\n"
		"%%llx(%s) = %llx\n"
		"%%X(%s) = %X\n"
		"%%llX(%s) = %llX\n"
		"%%llX(%s) = %llX\n"
		"%%o(%s) = %o\n",
		
		"123", 123,
		"0", 0,
		"-123", -123,
		"LONG_MIN", LONG_MIN,
		"25748453", 25748453,
		"LONG_MAX - 13492", LONG_MAX - 13492,
		"LONG_MAX", LONG_MAX,
		"LONG_LONG_MIN", LONG_LONG_MIN,
		"LONG_LONG_MIN + 19430", LONG_LONG_MIN + 19430,
		"LONG_LONG_MAX - 19282", LONG_LONG_MAX - 19282,
		"LONG_LONG_MAX", LONG_LONG_MAX,		
		"0b01000001", 0b01000001,
		"\"abrashvabrakadabra\"", "abrashvabrakadabra",
		"'0'", '0',
		
		"0xdeadbeef", 0xdeadbeef,
		"0xabacabadabacabaf", 0xabacabadabacabaf,
		"0xfefafcfdaf0feff", 0xfefafcfdaf0feff,
		"0xDEDDED29", 0xDEDDED29,
		"0xE9234EFBA9C0FAE", 0xE9234EFBA9C0FAE,
		"0xCABCADCAFCAECAA", 0xCABCADCAFCAECAA,
		"012321734", 012321734                            // Octal as zero is first digit.
	);
	return 0;
}