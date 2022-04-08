%include "../../2-05-printf/winapi.inc"

; TODO: удалить отладочную информацию.

global __start

extern GetStdHandle
extern ReadFile
extern SetConsoleOutputCP
extern GetProcessHeap
extern HeapAlloc
extern HeapFree
extern ExitProcess

extern myprintf
extern mystrlen

extern BCryptOpenAlgorithmProvider
extern BCryptGetProperty
extern BCryptCreateHash
extern BCryptHashData
extern BCryptFinishHash
extern BCryptCloseAlgorithmProvider
extern BCryptDestroyHash


CODE_PAGE equ 65001

section .text
validate_handle:
		cmp rax, INVALID_HANDLE_VALUE
		xor rcx, rcx
		sub rsp, 32
		je ExitProcess
		add rsp, 32
		ret
		
section .text
__start:
		mov rcx, CODE_PAGE
		sub rsp, 32
		call SetConsoleOutputCP
		add rsp, 32
		
		call main

		mov rcx, rax
		sub rsp, 32
		call ExitProcess
		add rsp, 32
		
section .text
; 0 -- undefined behaviour (it will fill all memory starting from rcx with dl.)
mymemset:
		push rdi
		push rax

		mov rdi, rcx
		mov rax, rdx
		mov rcx, r8
		rep stosb
		
		pop rax
		pop rdi
		ret

section .text
; 0 -- undefined behaviour (it will fill all memory starting from rcx with dl.)
mystrcmp:
		push rdi
		push rsi

		push rcx
		push rdx
		
		call mystrlen
		mov r8, rax
		
		mov rcx, [rsp]
		call mystrlen
		mov r9, rax
		
		cmp r8, r9
		jne .on_different_lengths
		
		mov rcx, r8
		pop rdi
		pop rsi

		repne scasb
		dec rsi
		dec rdi
		
		movzx rcx, byte [rsi]
		movzx rdx, byte [rdi]
		sub rcx, rdx
		
		pop rsi
		pop rdi
		
		mov rax, rcx
		ret
		
.on_different_lengths:
		mov rax, r8
		sub rax, r9
		
		pop rsi
		pop rdi
		ret

section .text
create_sha512_hash:
		push rcx                      ; buffer address
		push rdx                      ; size of buffer

		sub rsp, 32
		call GetProcessHeap
		add rsp, 32
		
		test rax, rax
		jz .hashing_failed
		
		mov [rel process_heap], rax

		lea rcx, [rel algo_handle]
		lea rdx, [rel sha512_alg_prop]
		xor r8, r8
		xor r9, r9
		sub rsp, 32
		call BCryptOpenAlgorithmProvider
		add rsp, 32
		
		test rax, rax
		js .hashing_failed                ; failed if negative
		
		; Getting has object size
		mov rcx, [rel algo_handle]
		lea rdx, [rel hash_object_size_prop]
		lea r8, [rel hash_object_size]
		mov r9, 8
		push 0
		push 0
		sub rsp, 32
		call BCryptGetProperty
		add rsp, 32
		add rsp, 16
		
		test rax, rax
		js .hashing_failed
		
		mov rcx, [rel process_heap]
		xor rdx, rdx
		mov r8, [rel hash_object_size]
		sub rsp, 32
		call HeapAlloc
		add rsp, 32
		
		test rax, rax
		jz .hashing_failed
		
		mov [rel hash_object_ptr], rax
		
		mov rcx, [rel algo_handle]
		lea rdx, [rel hash_len_prop]
		lea r8, [rel hash_str_len]
		mov r9, 4
		push 0
		push 0
		sub rsp, 32
		call BCryptGetProperty
		add rsp, 32
		add rsp, 16
		
		test rax, rax
		js .hashing_failed
		
		cmp dword [rel hash_str_len], EXPECTED_HASH_LEN
		jne .hashing_failed
		
		mov rcx, [rel process_heap]
		xor rdx, rdx
		mov r8, [rel hash_str_len]
		sub rsp, 32
		call HeapAlloc
		add rsp, 32
		
		test rax, rax
		jz .hashing_failed
		
		mov [rel hash_str_ptr], rax
		
		mov rcx, [rel algo_handle]
		lea rdx, [rel hashing_handle]
		mov r8, [rel hash_object_ptr]
		mov r9, [rel hash_object_size]
		push 0
		push 0
		push 0
		sub rsp, 32
		call BCryptCreateHash
		add rsp, 32
		add rsp, 24
		
		test rax, rax
		js .hashing_failed

		mov rcx, [rel hashing_handle]
		mov rdx, [rsp+8]              ; address of the buffer
		mov r8, [rsp]                 ; size of the buffer
		xor r9, r9
		sub rsp, 32
		call BCryptHashData
		add rsp, 32
		
		test rax, rax
		js .hashing_failed
		
		mov rcx, [rel hashing_handle]
		mov rdx, [rel hash_str_ptr]
		mov r8, [rel hash_str_len]
		xor r9, r9
		sub rsp, 32
		call BCryptFinishHash
		add rsp, 32
		
		test rax, rax
		js .hashing_failed
		
		
		mov rcx, [rel algo_handle]
		sub rsp, 32
		call BCryptCloseAlgorithmProvider
		add rsp, 32
		
		mov rcx, [rel hashing_handle]
		sub rsp, 32
		call BCryptDestroyHash
		add rsp, 32
		
		mov rcx, [rel process_heap]
		xor rdx, rdx
		mov r8, [rel hash_object_ptr]
		sub rsp, 32
		call HeapFree
		add rsp, 32
		
		mov rax, [rel hash_object_ptr]

		pop rdx
		pop rcx
		ret
		
.hashing_failed:
		mov rax, 1
		sub rsp, 32
		call ExitProcess
		
section .bss
process_heap: resq 1
algo_handle: resq 1
hashing_handle: resq 1

hash_object_size: resq 1
hash_str_len: resd 1

hash_str_ptr: resq 1
hash_object_ptr: resq 1

section .rdata
sha512_alg_prop: dw "SHA512"
hash_object_size_prop: dw "ObjectLength"
hash_len_prop: dw "HashDigestLength"

section .text
main:
		lea rcx, [rel greeting_fmt]
		mov rdx, rsp
		sub rsp, 32
		call myprintf
		add rsp, 32
		
		mov rcx, STD_INPUT_HANDLE
		sub rsp, 32
		call GetStdHandle
		add rsp, 32
		mov [rel stdin_handle], rax
		call validate_handle

		sub rsp, BUFFER_SIZE
		mov [rel buffer_ptr], rsp
		
		mov rcx, [rel buffer_ptr]
		xor dl, dl
		mov r8, BUFFER_SIZE
		sub rsp, 32
		call mymemset
		add rsp, 32
		
		mov rcx, [rel stdin_handle]
		mov rdx, [rel buffer_ptr]
		mov r8, BUFFER_SIZE - 1 + BUFFER_OVERRUN
		xor r9, r9
		push 0
		sub rsp, 32
		call ReadFile
		add rsp, 32
		add rsp, 8
		
		;--------------------------------------------------------------------
		; Returns length of a null terminated string.
		;
		; Entry: RCX - address of the start of the string
		; Note:  max string length is 2^64 - 1.
		; Exit:  RAX - length of the string
		; Chngs: RAX RCX
		;--------------------------------------------------------------------
		; size_t strlen( const char * string );
		mov rcx, [rel buffer_ptr]
		; sub rsp, 32  ; not needed for my strlen, as it is not used anyway.
		call mystrlen
		; add rsp, 32

		mov rcx, [rel buffer_ptr]
		mov rdx, rax
		call create_sha512_hash
		
		mov rcx, [rel print_password_fmt]
		mov rdx, rax
		sub rsp, 32
		call myprintf
		add rsp, 32
		
		xor rax, rax
		sub rsp, 32
		call ExitProcess
		
		call mystrcmp
		
		lea rcx, [rel print_password_fmt]
		mov rdx, [rel buffer_ptr]
		sub rsp, 32
		call myprintf
		add rsp, 32

		add rsp, BUFFER_SIZE

		xor rax, rax
		ret

section .bss
stdin_handle: resq 1

BUFFER_SIZE equ 8    ; Make sure it is devisible by 8.
BUFFER_OVERRUN equ 8 ; Some overflow potential.
buffer_ptr: resq 1

EXPECTED_HASH_LEN equ 128

buffer_hash: resb EXPECTED_HASH_LEN

section .rdata
; Не %p, т.к. он не поддерживается моим printf.
greeting_fmt: db "Привет, 0x%016llx. Ты меня вызывал?", 0xA, 0
password_prompt: db "Да. Мой пароль: ", 0
print_password_fmt: db "%s", 0
; sha512sum password.txt
expected_hash: db "97cf1e35d3a728597ba441b4886c60cd7ce097843f4013f9f9ee1db6d800606ff8ac2fc8a9170708379b3017b4b9abce14222728c7691a0260197e54079caa13"