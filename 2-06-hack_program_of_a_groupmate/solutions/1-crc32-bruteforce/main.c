#include <stdio.h>
#include <stddef.h>
#include <intrin.h>

int main() {
	FILE* output_file = fopen("input.txt", "wb");
	if (output_file == NULL) {
		return 1;
	}

	// Хеш должен быть равномерный, а его размер 32 бита. Можно перебрать.
	// С небольшим запасом, чтобы был побольше шанс встретить значение, поравномернее попринимать значения.
	static const unsigned int DESIRED_HASH = 0x0D26F461F;
	for (size_t nbytes = 0; nbytes < 5; ++nbytes) {	
		for (size_t i = 0; i < (((size_t) 1) << (8 * nbytes)); ++i) {
			unsigned int hash = 0;
			for (size_t j = 0; j < nbytes; ++j) {
				hash = _mm_crc32_u8(hash, (unsigned char) ((i >> (8 * j)) & 0xFF));
			}
			if (hash == DESIRED_HASH) {
				fwrite(&i, sizeof(char), nbytes, output_file);
				printf("num_bytes = %zu, i = 0x%016llx\n", nbytes, i);
				break;
			}
		}
	}
	
	fclose(output_file);
	return 0;
}