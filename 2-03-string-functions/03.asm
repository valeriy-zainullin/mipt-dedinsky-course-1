locals

;--------------------------------------------------------------------
; Returns length of a null terminated string.
;
; Entry: ES:DI - address of the start of the string
; Note:  max string length is 65535.
; Exit:  AL = 0
;        CX - length of the string
;        DI - next byte after the zero byte of the string
; Chngs: AL CX DI
;--------------------------------------------------------------------
; size_t strlen( const char * string );
strlen proc
		cld
		
		xor al, al
		mov cx, 0ffffh
		repne scasb         ; After the instruction cx is 0xFFFF - (number of non zero symbols + 1).
		                    ; cx is decremeted first, then comparison for cx = 0 or zf happens.
		                    ; after the zero byte cx is decremented as well.
		                    ; scasb after comparing zero byte will increment di as well.
		                    ; DI = next byte after the terminating zero byte.
		xor cx, 0ffffh      ; CX = ~CX = 0xFFFF - cx = number of non zero symbols + 1
		dec cx              ; CX = number of non zero symbols

		ret
endp

;--------------------------------------------------------------------
; Looks for character in a null-terminated string.
;
; Entry: ES:DI - address of the start of the string,
;        AL    - character to be found
; Note:  max string length is 65535.
; Exit:  AH = 0,
;        DI - address of the character found or zero
;        CX - length - index of character - 1 (0-based) or 0 if not found
; Chngs: AX CX DI
;--------------------------------------------------------------------
; char * strchr( const char * string, int symbol);
strchr proc
		cld
		
		xchg ah, al
		call strlen
		xchg ah, al
		
		test cx, cx
		jz @@not_found   ; taken if length is zero to not feed it to rep**. It will iterate over 0xFFFF + 1 items otherwise.
		
		; DI = address of the byte after the terminating zero.
		dec di
		sub di, cx
		; CX = start of the string

		repne scasb      ; After comparing the zero byte, di is still incremented.
		dec di           ; Decrement it back.
		
		test cx, cx
		jz @@not_found
		
		ret

@@not_found:
		xor di, di
		ret
endp

;--------------------------------------------------------------------
; Copies string to a buffer. At most num bytes written to the buffer.
;
; Entry: ES:DI - destination address
;        DS:SI - source address
;        BX    - max number of bytes written for dest. address
; Note:  max string length is 65535.
; Exit:   = 0,
;        DI - address of the character found or zero
;        CX - length - index of character - 1 (0-based)
; Chngs: AX BX CX SI DI
;--------------------------------------------------------------------
; char * strncpy( char * destptr, const char * srcptr, size_t num );
; Rethink. May be broken as hell.
strncpy proc
		cld
		
		mov ax, ds
		mov cx, es
		mov ds, cx
		mov es, ax
		xchg di, si

		;--------------------------------------------------------------------
		; Returns length of a null terminated string.
		;
		; Entry: ES:DI - address of the start of the string
		; Note:  max string length is 65535.
		; Exit:  AL = 0
		;        CX - length of the string
		;        DI - next byte after the zero byte of the string
		; Chngs: AL CX DI
		;--------------------------------------------------------------------
		; size_t strlen( const char * string );
		call strlen
		inc cx                                              ; CX = length + 1
		; SI is at the byte after the zero byte.
		; sub di, cx

		cmp cx, bx
		jb @@fill_with_zeros
		ja @@limit_output
		jmp @@do_copy

@@fill_with_zeros:
		sub bx, cx

		add di, bx
		xchg bx, cx
		std
		xor al, al
		rep stosb
		cld

		xchg bx, cx
		
		; DI is now shifted for -(1 + (BX - CX)) at the start of the string as before.
		
		jmp @@do_copy

@@limit_output:
		mov cx, bx
		jmp @@do_copy
		
@@do_copy:
		mov ax, ds
		mov bx, es
		mov ds, bx
		mov es, ax
		xchg di, si
		
		rep movsb
		
		ret
endp

;--------------------------------------------------------------------
; Compares two strings. Considers first n bytes.
;
; Entry: ES:DI - first string
;        DS:SI - second string
;        BX    - number of bytes considered
; Note:  max string length is 65535.
; Exit:   = 0,
; Chngs: AH CX DI
;--------------------------------------------------------------------
; int strncmp( const char * string1, const char * string2, size_t num );
strncmp proc
		cld
		
		call strlen
		
		
		
		repe cmpsb
endp

;---------------------------------------------------------------------------------
; Converts integer to ASCII string in the specified base.
;
; Entry: AX - number to be converted,
;        BX - base,
;        ES:DI - output string (must be sufficient to hold the number converted).
; Note:  none.
; Exit:  DS:DI - address of the zero byte of the string
; Chngs: BX DX
;---------------------------------------------------------------------------------
; void* itoa(int input, char *buffer, int radix)
; TODO: handle negative values.
; TODO: optimize a little bit more.
itoa proc

		push si

		; UB if base if less than 2 or greater than 36
		; cmp bx, 2
		; jl @@invalid_base
		; cmp bx, 36
		; jg @@invalid_base
		
		push di

		test ax, ax
		jz @@number_is_zero

		mov dx, bx
		dec dx
		test dx, bx
		
		jz @@power_of_2

; General base
@@general_base_loop:
		test ax, ax
		jz @@loop_end
		
		xor dx, dx
		div bx
		
		mov si, dx
		xor dh, dh
		mov dl, @@char_table[si * 1]
		
		mov es:[di], dx
		inc di
		
		jmp @@general_base_loop

@@power_of_2:
		xor cl, cl
@@power_loop:
		cmp bx, 1
		je @@power_calculated
		
		inc cl
		shr bx, 1
		
		jmp @@power_loop
@@power_calculated:

		shl bx, cl
		dec bx
@@power_of_2_loop:
		test ax, ax
		jz @@loop_end
		
		mov si, ax
		and si, bx
		xor dh, dh
		mov dl, @@char_table[si * 1]
		
		mov es:[di], dx
		inc di
		
		shr ax, cl
		jmp @@power_of_2_loop
		
@@loop_end:

		mov byte ptr es:[di], 0
		
		pop si
		push di
		call reverse_string
		pop di
		
		pop si

		ret

@@number_is_zero:
		pop si
		
		mov al, '0'
		stosb
		xor al, al
		stosb
		
		pop si
		ret

;@@invalid_base:
;		pop di
;		ret

@@char_table db '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
             db 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'
             db 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T'
             db 'U', 'V', 'W', 'X', 'Y', 'Z'

endp
	
;
; Entry: ES:SI - start of the string
;        ES:DI - address of the zero byte in the string
;
reverse_string proc
		dec di
		
		push dx
		
@@loop:
		cmp si, di
		jae @@loop_end
		
		mov dl, es:[si]		
		mov dh, es:[di]
		xchg dl, dh
		mov es:[si], dl
		mov es:[di], dh
		
		inc si
		dec di
		
		jmp @@loop
@@loop_end:

		pop dx

		ret
endp

;---------------------------------------------------------------------------------
; Converts ASCII string in base 10 to string.
;
; Entry: AX - number to be converted,
;        BX - base,
;        DS:SI - output string (must be sufficient to hold the number converted).
; Note:  none.
; Exit:  DS:SI - address of the zero byte of the string
; Chngs: BX DX
;---------------------------------------------------------------------------------
; void* itoa(int input, char *buffer, int radix)
atoi proc
endp
