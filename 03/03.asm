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
; ???
;
; Entry: ES:DI - address of the start of the string,
;        AL    - character to be found
; Note:  max string length is 65535.
; Exit:  AH = 0,
;        DI - address of the character found or zero
;        CX - length - index of character - 1 (0-based)
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
; ???
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
		sub di, cx

		cmp cx, bx
		jbe @@skip_limiting_output
		
		mov cx, bx
@@skip_limiting_output:

		mov ax, ds
		mov bx, es
		mov ds, bx
		mov es, ax
		xchg di, si
		
		rep movsb
		
		ret
endp

;--------------------------------------------------------------------
; ???
;
; Entry: ES:DI - destination address
;        DS:SI - source address
;        BX    - max number of bytes con
; Note:  max string length is 65535.
; Exit:   = 0,
;        DI - address of the character found or zero
;        CX - length - index of character - 1 (0-based)
; Chngs: AH CX DI
;--------------------------------------------------------------------
; int strncmp( const char * string1, const char * string2, size_t num );
strncmp proc
		cld
		
		
		
		repe cmpsb
endp

itoa proc
endp

atoi proc
endp
