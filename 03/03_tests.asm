.model tiny

.code
org 100h

locals

start:
		mov ax, ds
		mov es, ax
		
		mov di, offset string_copy
		mov si, offset string
		mov bx, 7
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
		; Chngs: AH CX SI DI
		;--------------------------------------------------------------------
		; char * strncpy( char * destptr, const char * srcptr, size_t num );
		call strncpy

		mov ax, 4c00h                            ; Выйти из программы и передать управление DOSу
		int 21h

include 03.asm
endp
		
.data

string db "12345678", 0
string_copy db "01010101010", 0

; size_t strlen( const char * string );
; char * strchr( const char * string, int symbol);
; char * strncpy( char * destptr, const char * srcptr, size_t num );
; int strncmp( const char * string1, const char * string2, size_t num );
; char *itoa( int value, char * string, int radix );
; int atoi( const char * string );

; strlen, strchr, strncpy, strncmp, itoa, atoi, printf

end start