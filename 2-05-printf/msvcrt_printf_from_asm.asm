%include "winapi.inc"

; kernel32.dll
; 2022.03.21: libkernel32.a used by x86_64-w64-mingw32-gcc has functions (not one, BaseCheckRunApp, for example)
;             not exported by kernel32.dll anymore. Looks like it wasn't updated yet. The program won't start
;             because of this.
; I directly link with a dll in this case. It is still dynamically linked (easy to check with pe-bear). I think
; it just checks the functions are there, multiple definitions from dlls maybe, undefined references and writes
; the name of the dll into the import table.
; Maybe the missing functions in one of crt*.o files. I saw mingw-ld linking these when $(which kernel32.dll)
; is specified in objects to link. Just why -lkernel32 works for C programs? Nevermind. This may mean that
; these libraries just not intented to be used with assembly objects (or without C objects).

; msvcrt.dll
extern printf

; kernel32.dll
extern ExitProcess

global __start

section .text

__start:
		mov rcx, format_string
		mov edx, -27284213
		mov r8d, -123456789
		mov r9d, 123490809
		; The byte code will contain 64-bit value. The operand is treated as signed and nasm here will
		; sign-expand it to the 64 bits. It's not distinguishable from pushing qword on the stack (what was
		; pushed qword or dword), but it is not distinguishable for printf anyway, it will figure out the
		; value by looking at the format string.
		; DWORD here is about preparing the value to fill 64 bits before pushing. The same for byte, as I
		; found in the internet (there was the same issue with 32-bits and a byte, so I think I deal with
		; the same here).
		; https://groups.google.com/g/comp.lang.asm.x86/c/3A88dt_uPYk
		;push dword 0xDED9DED9
		;push dword 0xded9ded9
		; ---
		; In 16-bit mode, you can push 16 or (with operand-size prefix on 386 and later) 32 bits.
		; In 32-bit mode, you can push 32 or (with operand-size prefix) 16 bits.
		; In 64-bit mode, you can push 64 or (with operand-size prefix) 16 bits.
		; https://stackoverflow.com/a/45134007
		; And after that I had found out that there one can't push even 64 bits immediate in 64-bit mode.
		; Only 32-bit. Maybe it was done to reduce redundancy. Just use mov and sub after, and everything
		; is done. Yes, as it is needed to store arguments, but then there are many of them likely, so
		; plus one intruction is not a problem, but less redundancy. And registers are still possible to
		; be saved.
		; Stack is always aligned on 8-byte boundary, we effectively aren't pushing a dword here.
		; ---
		; Can't do this either. The only x86-64 instruction that can use a 64-bit immediate is mov-immediate
		; to register, the special no-modrm opcode that gives us 5-byte mov eax, 12345, or 10-byte
		; mov rax, 0x0123456789abcdef, where a REX.W prefix changes that opcode to look for a 64-bit immediate.
		; See https://www.felixcloutier.com/x86/mov / why we can't move a 64-bit immediate value to memory?
		;mov qword [rsp-16], 0xDED9DED9
		;mov qword [rsp-8], 0xded9ded9
		; https://stackoverflow.com/a/70731545
		; ---
		; I have to use a temporary register. Maybe because storing contants is not a very often operations
		; and it doesn't break pipelining, so not a big deal for another command involved. And it can be
		; stored in register if it is used for a long time, there are more registers now, maybe...
		mov r14d, 0xDED9DED9
		push r14
		mov r14d, 0xded9ded9
		push r14
		push dword 0o7772364510

		sub rsp, 32
		call printf
		add rsp, 32

		xor rcx, rcx
		sub rsp, 32
		call ExitProcess       ; ExitProcess (0)
		add rsp, 32

section .data

LF equ 0xA ; '\n'
format_string: db "%d %i %u %o %x %X ", LF, 0, "%f %F %e %E %g %G %a %A %c %s %p %n %%\n", 0
