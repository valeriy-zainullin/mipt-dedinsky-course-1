input_bytes = list(open("nHack.orig.exe", "rb").read())
je_va = 0x140001057
section_va = 0x140001000
section_ra = 0x400
je_ra = (je_va - section_va) + section_ra
# je in this case starts with 0x74, then goes the offset (relative to rip).
# Analogous jmp start with 0xEB, then goes the offset (relative to rip).
input_bytes[je_ra] = 0xEB
open("nHack.patched.exe", "wb").write(bytearray(input_bytes))
