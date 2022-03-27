.model tiny

.code

org 100h

ENTER_SCANCODE = 1ch
VIDEO_SEG = 0b800h

start:
		cld
		
		mov dx, VIDEO_SEG
		mov es, dx
		
		mov ax, 1003h
		mov bl, 00h
		int 10h

		xor di, di                    ; DI = current address in video segment
read_keypress_loop:
		xor ah, ah
		int 16h
		
		cmp ah, ENTER_SCANCODE
		je read_keypress_loop_end
		
		xor ah, ah                     ; AX = ascii code of the symbol.
		push ax
		
		xor ah, ah
		int 1ah                        ; int 1ah: read system clocks (ticks)
		
		pop ax
		mov ah, dl
		
		stosw
		
		jmp read_keypress_loop
read_keypress_loop_end:

		mov ax, 4c00h
		int 21h

end start