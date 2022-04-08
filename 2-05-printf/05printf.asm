%include "winapi.inc"

extern GetStdHandle
extern WriteFile

global myprintf
global mystrlen ; Использую в следующем задании.

section .text
;

;---------------------------------------------------------------------------------
; Writes character (ASCII) to the console.
;
; Entry: al - character
; Note:  none.
; Exit:  none.
; Chngs: RAX R11
;---------------------------------------------------------------------------------
; TODO: переделать в реализацию putc, переименовать putc.
write_char:
		mov [rel .char], al
		push rcx
		push rdx
		push r8
		push r9
		push r10
		mov rcx, [rel stdout_handle]
		lea rdx, [rel .char]
		mov r8, 1
		xor r9, r9
		push 0
		sub rsp, 32
		call WriteFile
		add rsp, 32
		add rsp, 8
		pop r10
		pop r9
		pop r8
		pop rdx
		pop rcx
		
		ret

section .bss
.char:  resb 1

section .text
;---------------------------------------------------------------------------------
; Converts unsigned integer to ASCII string in the specified base.
;
; Entry: RCX - number to be converted,
;        RDX - base,
;        R8 - output string (must be sufficient to hold the number converted).
; Note:  none.
; Exit:  none.
; Chngs: RCX R8 R9 R10
;---------------------------------------------------------------------------------
; void* itoa_unsigned(unsigned int input, char *buffer, int radix)
; TODO: optimize a little bit more.
myitoa_unsigned:
		;?push si

		; UB if base if less than 2 or greater than 36
		; cmp bx, 2
		; jl @@invalid_base
		; cmp bx, 36
		; jg @@invalid_base
		
		;?push di
		
		test rax, rax
		jz .number_is_zero
		
		push r8

		mov rax, rdx
		dec rax
		test rax, rdx
		
		jz .power_of_2
		
		mov rax, rcx
		mov r9, rdx                              ; R9 - base

; General base
.general_base_loop:
		test rax, rax
		jz .loop_end
		
		xor rdx, rdx
		div r9                                   ; RAX - quotient, RDX - remainder
		
		lea r10, [rel .char_table]
		movzx rdx, byte [r10 + rdx]
		
		mov [r8], dl
		inc r8
		
		jmp .general_base_loop

.power_of_2:
		mov r9, rdx
		dec r9                                   ; R9 - mask of last bits
		tzcnt rdx, rdx                           ; Equivalent of __builtin_ctz() in asm
		                                         ; similar to __builtin_popcnt() and popcnt.
		xchg rcx, rdx                            ; shr works only for cl as the second operand.
.power_of_2_loop:
		test rdx, rdx
		jz .loop_end
		
		mov rax, rdx
		and rax, r9
		
		lea r10, [rel .char_table]
		movzx r10, byte [r10 + rax]
		
		mov [r8], r10b
		inc r8
		
		shr rdx, cl
		
		jmp .power_of_2_loop
		
.loop_end:

		mov [r8], byte 0
		
		mov rdx, r8
		pop rcx 
		call reverse_string

		ret

.number_is_zero:
		mov [r8], byte '0'		
		inc r8
		
		mov [r8], byte 0

		ret

;@@invalid_base:
;		pop di
;		ret

section .rdata
.char_table:
	db '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
	db 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'
	db 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T'
	db 'U', 'V', 'W', 'X', 'Y', 'Z'

section .text
; Entry: RCX - start of the string
;        RDX - address of the zero byte in the string
;
reverse_string:
		dec rdx
.loop:
		cmp rcx, rdx
		jae .loop_end
		
		mov al, [rcx]
		mov ah, [rdx]
		mov [rcx], ah
		mov [rdx], al

		inc rcx
		dec rdx
		
		jmp .loop

.loop_end:

		ret

;---------------------------------------------------------------------------------
; Converts integer to ASCII string in the specified base.
;
; Entry: RCX - number to be converted,
;        RDX - base,
;        R8 - output string (must be sufficient to hold the number converted).
; Note:  none.
; Exit:  none.
; Chngs: RCX R8 R9 R10
;---------------------------------------------------------------------------------
section .text
myitoa:
		test rcx, rcx
		js .number_is_negative
		
		call myitoa_unsigned
		ret

.number_is_negative:
		mov [r8], byte '-'
		inc r8
		not rcx
		inc rcx                  ; From 2's complement to unsigned value.
		call myitoa_unsigned
		ret

section .text
; Entry: RCX - character
;
tolower:
		cmp rcx, 'A'
		jb .not_converted
		
		cmp rcx, 'Z'
		ja .not_converted
		
		add rcx, -'A'+'a'
		
		mov rax, rcx
		
		ret

.not_converted:
		mov rax, rcx
		ret

section .text
; Entry: RCX - start of the string
;        RDX - address of the zero byte in the string
;
tolower_str:
		mov r8, rcx
.loop:
		cmp r8, rdx
		jae .loop_end
		
		movzx rcx, byte [r8]
		call tolower
		mov [r8], al
		
		inc r8
		
		jmp .loop

.loop_end:

		ret

section .text
;--------------------------------------------------------------------
; Returns length of a null terminated string.
;
; Entry: RCX - address of the start of the string
; Note:  max string length is 2^64 - 1.
; Exit:  RAX - length of the string
; Chngs: RAX RCX
;--------------------------------------------------------------------
; size_t strlen( const char * string );
mystrlen:
		push rdi
		mov rdi, rcx

		cld
		xor al, al
		mov rcx, -1
		repne scasb         ; After the instruction rcx is 0xFFFFFFFFFFFFFFFF - (number of non zero symbols + 1).
		                    ; rcx is decremeted first, then comparison for rcx = 0 or zf happens.
		                    ; after the zero byte cx is decremented as well.
		                    ; scasb after comparing zero byte will increment rdi as well.
		                    ; RDI = next byte after the terminating zero byte.
		xor rcx, -1         ; RCX = ~rcx = 0xFFFF... - rcx = number of non zero symbols + 1
		dec rcx             ; RCX = number of non zero symbols
		
		pop rdi
		
		mov rax, rcx
		ret

section .text
;--------------------------------------------------------------------
; Writes null-terminated string to the console.
; Additionally support zero-padding. There is no such argument
; in standard puts.
;
; Entry: RCX - address of the start of the string
;        RDX - expected length of the string, front is padded with
;              zeros the front to achive it (if possible)
; Note:  none
; Exit:  none
; Chngs: RCX RDX
;--------------------------------------------------------------------
; ... puts( ... );
myprintf_puts:
		push rax
		push rcx
		push rdx
		push r8
		push r9
		push r10
		push r11

		push rcx
		call mystrlen
		pop rcx
		
		cmp rax, rdx
		jae .skip_zero_padding
		
		sub rdx, rax
.loop:
		test rdx, rdx
		jz .loop_end
		
		push rax
		mov al, '0'
		push r11
		call write_char
		pop r11
		pop rax
		
		dec rdx
		
		jmp .loop
.loop_end:

.skip_zero_padding:
		
		mov rdx, rcx
		mov rcx, [rel stdout_handle]
		mov r8, rax
		xor r9, r9
		push 0
		sub rsp, 32
		call WriteFile
		add rsp, 32
		add rsp, 8

		pop r11
		pop r10
		pop r9
		pop r8
		pop rdx
		pop rcx
		pop rax
		
		ret
		
section .text
extract_signed_int_arg:
		; neg rax
		; [base + scale*index + displacement],
		; base is any gen. purp register,
		; scale is 2, 4, 8, index is any gen. purp registers except esp, rsp,
		; displacement is 8-, 16- or 32-bit value.
		; Any of the summands may be ommited.
		cmp r10, SIZE_DEFAULT
		je .extract_int
		
		cmp r10, SIZE_LONG
		je .extract_long
		
		cmp r10, SIZE_LONGLONG
		je .extract_longlong
.extract_int:
.extract_long:
		movsx rax, dword [rbp+8*rax+24]
		ret

.extract_longlong:
		mov rax, [rbp+8*rax+24]
		ret

section .text
read_fmt_str_char:
		movzx rax, byte [rcx]
		inc rcx
		ret

section .text
extract_unsigned_int_arg:
		; neg rax
		; [base + scale*index + displacement],
		; base is any gen. purp register,
		; scale is 2, 4, 8, index is any gen. purp registers except esp, rsp,
		; displacement is 8-, 16- or 32-bit value.
		; Any of the summands may be ommited.
		cmp r10, SIZE_DEFAULT
		je .extract_int
		
		cmp r10, SIZE_LONG
		je .extract_long
		
		cmp r10, SIZE_LONGLONG
		je .extract_longlong
.extract_int:
.extract_long:
		mov eax, [rbp+8*rax+24]
		ret
		
.extract_longlong:
		mov rax, [rbp+8*rax+24]
		ret

section .text
extract_addr_arg:
		; neg rax
		; [base + scale*index + displacement],
		; base is any gen. purp register,
		; scale is 2, 4, 8, index is any gen. purp registers except esp, rsp,
		; displacement is 8-, 16- or 32-bit value.
		; Any of the summands may be ommited.
		mov rax, [rbp+8*rax+24]
		ret

section .text
process_flags:
		cmp rax, '0'
		je .read_zero_padding
		ret
.read_zero_padding:
		call read_fmt_str_char
.loop:
		cmp rax, '0'
		jb .loop_end
		
		cmp rax, '9'
		ja .loop_end
		
		; Multiplication by 10. In my opinion, it is easier than
		; saving rdx:rax, then moving values, then using mul.
		; Utilizing binary representation of 10.
		push r13
		mov r13, r12
		shl r12, 3
		shl r13, 1
		add r12, r13
		pop r13
		
		sub rax, '0'
		add r12, rax
		
		call read_fmt_str_char
		jmp .loop
.loop_end:
		ret
		
		
section .text
process_size_specs:
		cmp rax, 'z'
		je .set_size_size_t
		
		cmp rax, 'l'		
		je .process_longs
		
		ret
.set_size_size_t:
		mov r10, SIZE_SIZE_T
		call read_fmt_str_char
		ret
.process_longs:
		mov r10, SIZE_LONG
				
		call read_fmt_str_char
		cmp rax, 'l'
		je .set_size_longlong
		
		ret
.set_size_longlong:
		mov r10, SIZE_LONGLONG
		call read_fmt_str_char
		ret

section .text
myprintf:
		mov [rsp+16], rdx
		mov [rsp+24], r8
		mov [rsp+32], r9
		push rbp
		mov rbp, rsp
		push r12
		; RCX - pointer to the format string
		; Arguments are on the stack in shadow space from calling convention. It is faster
		; than jump table for argument index and acquision from register and frees some
		; registers.
		; RAX, RCX, RDX, R8, R9, R10, R11 are caller saved.
		; RDX - number of characters written so far
		; R8 - current specifier index
		; R10 - size of the specifier
		; R12 - zero padding to the specified length or zero if not present
		; RAX, R9 - tmp registers. RAX is often the current character.
		; If zero padding is less than length of non-zero digits, it has no offect, so technically to the least geq length.
		; Zero padding is ignored for chars (simplification, if it is supported by standard printf).
		; Zero padding works correctly only for non-negative values.
		; Encoding is ASCII. If other encoding is in use, the bytes with ASCII numbers
		; for '%', 'c', 'd', ..., '\n', '\r' will be treated as ASCII instead of encoding
		; in use. Other chars (should? be) printed as bytes.
		xor r8, r8
		xor rdx, rdx
		call get_stdout_handle
		test rax, rax
		jz .failure
.loop:
		call read_fmt_str_char
		test rax, rax
		jz .loop_end
		
		cmp rax, '%'
		je .process_spec

		call write_char
		jmp .loop_next_iteration

.process_spec:
		; Get next char.
		call read_fmt_str_char
		
		xor r12, r12
		call process_flags

		mov r10, SIZE_DEFAULT
		call process_size_specs
		
		; if rax was less than '%', then treating is as an unsigned, it will be greater than 'o' - '%'
		; (-'%' would not overflow signed 64-bit, 'o' would not overflow signed 64-bit, then the
		; inequality holds, as any negative value is greater than any positive value if signed integer
		; converted to an unsigned). 
		sub rax, FIRST_SPEC
		cmp rax, LAST_SPEC - FIRST_SPEC
		ja .process_inv_spec
		lea r11, [rel .printf_jump_table]
		mov rax, [r11 + 8 * rax]
		jmp rax

.process_percent:
		mov rax, '%'
		call write_char
		test rax, rax
		; ...2
		jmp .loop_next_iteration
		
.process_binary:
		mov rax, r8
		inc r8
		call extract_signed_int_arg
		
		push rcx
		push rdx
		push r8
		push r9
		push r10
		;		RCX - number to be converted,
		;        RDX - base,
		; R8 - output string (must be sufficient to hold the number converted).
		mov rcx, rax
		mov rdx, 2
		mov r8, .number
		call myitoa
		pop r10
		pop r9
		pop r8
		pop rdx
		pop rcx
		
		push rcx
		push rdx
		mov rcx, .number
		mov rdx, r12
		call myprintf_puts
		pop rdx
		pop rcx
		
		jmp .loop_next_iteration

.process_char:
		mov rax, r8
		inc r8
		call extract_signed_int_arg
		
		call write_char
		
		jmp .loop_next_iteration

.process_int:
		mov rax, r8
		inc r8
		call extract_signed_int_arg

		push rcx
		push rdx
		push r8
		push r9
		push r10
		;		RCX - number to be converted,
		;        RDX - base,
		; R8 - output string (must be sufficient to hold the number converted).
		mov rcx, rax
		mov rdx, 10
		mov r8, .number
		call myitoa
		pop r10
		pop r9
		pop r8
		pop rdx
		pop rcx
		
		push rcx
		push rdx
		mov rcx, .number
		mov rdx, r12
		call myprintf_puts
		pop rdx
		pop rcx

		jmp .loop_next_iteration

.process_octal:
		mov rax, r8
		inc r8
		call extract_unsigned_int_arg
		
		push rcx
		push rdx
		push r8
		push r9
		push r10
		;		RCX - number to be converted,
		;        RDX - base,
		; R8 - output string (must be sufficient to hold the number converted).
		mov rcx, rax
		mov rdx, 8
		mov r8, .number
		call myitoa_unsigned
		pop r10
		pop r9
		pop r8
		pop rdx
		pop rcx
		
		push rcx
		push rdx
		mov rcx, .number
		mov rdx, r12
		call myprintf_puts
		pop rdx
		pop rcx

		jmp .loop_next_iteration

.process_string:
		mov rax, r8
		inc r8
		call extract_addr_arg
		
		push rcx
		push rdx
		mov rcx, rax
		mov rdx, r12
		call myprintf_puts
		pop rdx
		pop rcx
		
		jmp .loop_next_iteration

.process_lowercase_hex:
		mov rax, r8
		inc r8
		call extract_unsigned_int_arg
		
		push rcx
		push rdx
		push r8
		push r9
		push r10
		;		RCX - number to be converted,
		;        RDX - base,
		; R8 - output string (must be sufficient to hold the number converted).
		mov rcx, rax
		mov rdx, 16
		mov r8, .number
		call myitoa_unsigned
		
		mov rdx, r8
		mov rcx, .number
		call tolower_str
		
		pop r10
		pop r9
		pop r8
		pop rdx
		pop rcx
		
		push rcx
		push rdx
		mov rcx, .number
		mov rdx, r12
		call myprintf_puts
		pop rdx
		pop rcx

		jmp .loop_next_iteration
		
.process_uppercase_hex:
		mov rax, r8
		inc r8
		call extract_unsigned_int_arg
		
		push rcx
		push rdx
		push r8
		push r9
		push r10
		;		RCX - number to be converted,
		;        RDX - base,
		; R8 - output string (must be sufficient to hold the number converted).
		mov rcx, rax
		mov rdx, 16
		mov r8, .number
		call myitoa_unsigned
		pop r10
		pop r9
		pop r8
		pop rdx
		pop rcx
		
		push rcx
		push rdx
		mov rcx, .number
		mov rdx, r12
		call myprintf_puts
		pop rdx
		pop rcx

		jmp .loop_next_iteration

.process_inv_spec:

.loop_next_iteration:		
		jmp .loop
		
		
.loop_end:

.failure:
		pop r12
		pop rbp
		ret

section .rdata

SIZE_DEFAULT  equ 0
SIZE_SIZE_T   equ 1
SIZE_LONG     equ 2
SIZE_LONGLONG equ 4

FIRST_SPEC equ '%'
LAST_SPEC equ 'x'
; '%', 'b', 'c', 'd', 'o', 's'
.printf_jump_table:
	dq .process_percent                                    ; '%'
	times ('X' - '%' - 1) dq .process_inv_spec             ; ('%', 'X') = ('X', 0] / ('%', 0] / {'%'}
	dq .process_uppercase_hex                              ; 'X'
	times ('b' - 'X' - 1) dq .process_inv_spec             ; ('X', 'b') = ('b', 0] / ('X', 0] / {'X'}
	dq .process_binary                                     ; 'b'
	dq .process_char                                       ; 'c'
	dq .process_int                                        ; 'd'
	times ('o' - 'd' - 1) dq .process_inv_spec             ; ('d', 'o') = ('o', 0] / ('d', 0] / {'d'}
	dq .process_octal                                      ; 'o'
	times ('s' - 'o' - 1) dq .process_inv_spec             ; ('o', 's') = ('s', 0] / ('o', 0) / {'o'}
	dq .process_string                                     ; 's'
; TODO: добавить unsigned (в задачу не входило, она упрощённая).
;	times ('u' - 's' - 1) dq .process_inv_spec             ; ('o', 's') = ('s', 0] / ('o', 0) / {'o'}
;	dq .process_unsigned                                   ; 'u'
	times ('x' - 's' - 1) dq .process_inv_spec             ; ('s', 'x') = ('x', 0] / ('s', 0) / {'s'}
	dq .process_lowercase_hex                              ; 'x'

section .data
.number: times (1 + 64 + 1) db 0


section .text
get_stdout_handle:
		push rcx
		push rdx
		push r8
		push r9
		
		mov rcx, STD_OUTPUT_HANDLE
		sub rsp, 32
		call GetStdHandle
		add rsp, 32
		cmp rax, INVALID_HANDLE_VALUE
		je .invalid_handle
		
		mov [rel stdout_handle], rax
		mov rax, 1

		pop r9
		pop r8
		pop rdx
		pop rcx
		ret
.invalid_handle:
		mov rax, 0

		pop r9
		pop r8
		pop rdx
		pop rcx
		ret

section .data
ferror_flag dq 0
		
section .bss
stdout_handle resq 1