input_bytes = list(open("main_ready.orig.exe", "rb").read())
jmp_short_va = 0x140001420
jmp_short_size = 2
section_va = 0x140001000
section_ra = 0x400
for i in range(jmp_short_size):
	input_bytes[jmp_short_va - section_va + section_ra + i] = 0x90
open("main_ready.patched.exe", "wb").write(bytearray(input_bytes))
