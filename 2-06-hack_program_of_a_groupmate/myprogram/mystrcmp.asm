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

		repe cmpsb
		dec rsi
		dec rdi
		
		movzx rcx, byte [rsi]
		movzx rdx, byte [rdi]
		sub rcx, rdx
		
		pop rsi
		pop rdi
		
		mov rax, rcx
		ret
