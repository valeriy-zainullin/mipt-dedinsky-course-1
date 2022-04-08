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
		"%%o(%s) = %o\n"

		"%%01d(%s) = %01d\n"
		"%%02d(%s) = %02d\n"
		"%%03d(%s) = %03d\n"
		"%%04ld(%s) = %04ld\n"
		"%%05ld(%s) = %05ld\n"
		"%%06ld(%s) = %06ld\n"
		"%%07ld(%s) = %07ld\n"
		"%%08lld(%s) = %08lld\n"
		"%%09lld(%s) = %09lld\n"
		"%%010lld(%s) = %010lld\n"
		"%%011lld(%s) = %011lld\n"
		"%%012b(%s) = %012b\n"
		"%%013s(%s) = %013s\n"
		"%%014c(%s) = %014c (no zero padding, implemendation specific)\n"
		// "%%c(%s) = %c\n" // TODO: print negative values.
		"%%015x(%s) = %015x\n"
		"%%016llx(%s) = %016llx\n"
		"%%017llx(%s) = %017llx\n"
		"%%018X(%s) = %018X\n"
		"%%019llX(%s) = %019llX\n"
		"%%020llX(%s) = %020llX\n"
		"%%021o(%s) = %021o\n",
		
		
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
		"012321734", 012321734,                           // Octal as zero is first digit.

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