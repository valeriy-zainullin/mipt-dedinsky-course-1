%include "../2-05-printf/winapi.inc"

extern GetStdHandle
extern ReadFile
extern WriteFile
extern ExitProcess

global __start

section .text
__start:
		xor rcx, rcx
		call ExitProcess
		
section .data
stdin_handle: dq 0
stdout_handle: dq 0