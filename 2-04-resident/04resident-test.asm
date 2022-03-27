.186
.model tiny
.code
locals
org 100h

start:
		mov ax, 0DED9h
		mov ds, ax
		
		mov ax, 0ABCDh
		mov es, ax
		
		mov ax, 12ABh
		mov bx, 34CDh
		mov cx, 56EFh
		mov dx, 7890h
		mov si, 0FEFAh
		mov di, 0BEEFh
	
@@char_loop:
		push ax
		in al, 60h
		
		cmp al, 01h
		je @@char_loop_end
		
		pop ax
		jmp @@char_loop
		
@@char_loop_end:
		mov ax, 4c00h
		int 21h

end start