.model tiny

.code
org 100h

locals

start:
		mov ax, ds
		mov es, ax
		
		mov ax, number
		mov bx, 16
		mov di, offset number_as_string
		
		;---------------------------------------------------------------------------------
		; Converts integer to ASCII string in the specified base.
		;
		; Entry: AX - number to be converted,
		;        BX - base,
		;        DS:SI - output string (must be sufficient to hold the number converted).
		; Note:  none.
		; Exit:  DS:SI - address of the zero byte of the string
		; Chngs: BX DX
		;---------------------------------------------------------------------------------
		call itoa

		mov ax, 4c00h                            ; Выйти из программы и передать управление DOSу
		int 21h

include 03.asm
endp
		
.data

number dw 12345
number_as_string db "abcefghijklmnopqrstuvxyz", 0

; size_t strlen( const char * string );
; char * strchr( const char * string, int symbol);
; char * strncpy( char * destptr, const char * srcptr, size_t num );
; int strncmp( const char * string1, const char * string2, size_t num );
; char *itoa( int value, char * string, int radix );
; int atoi( const char * string );

; strlen, strchr, strncpy, strncmp, itoa, atoi, printf

end start