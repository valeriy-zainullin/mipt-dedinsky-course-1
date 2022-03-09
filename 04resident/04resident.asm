.186
.model tiny
.code
locals
org 100h

start:
		mov bx, 8
		mov di, offset int8_jmp_far_arg
		mov si, offset int8_handler
		call set_int

		; mov bx, 9
		; mov di, offset int9_jmp_far_arg
		; mov si, offset int9_handler
		; call set_int

		call go_resident
		
;--------------------------------------------------------------------
; Go resident.
;
; Entry: None
; Note:  none
; Exit:  ES = 0
; Chngs: AX DX
;--------------------------------------------------------------------
go_resident proc
		mov ax, 3100h
		mov dx, offset end_of_program
		shr dx, 4                           ; Number of bytes in paragraphs (chunks of 16 bytes)
		inc dx                              ; Might not be a multiple of 16
		
		int 21h                             ; TSR: terminate and stay resident.
		
		ret                                 ; Never executed actually.
endp

;--------------------------------------------------------------------
; Changes entry in interrupt table, saving old value.
;
; Entry: BX    - interrupt number
;        DI    - address in ds segment to store offset and register
;        SI    - address in cs segment of the new handler
; Note:  none
; Exit:  ES = 0
; Chngs: AX BX ES
;--------------------------------------------------------------------
set_int proc
		cli
		
		xor ax, ax
		mov es, ax
		
		shl bx, 2                           ; multiply bx by 4 so that it is address of the offset.
		mov ax, es:[bx]
		mov [di], ax
		mov ax, es:[bx+2]
		mov [di+2], ax

		mov es:[bx], si
		mov es:[bx+2], cs
		
		sti
		
		ret
endp

;
; TODO.
;
save_registers proc
		mov register_table[0 * 2], ax
		mov register_table[1 * 2], bx
		mov register_table[2 * 2], cx
		mov register_table[3 * 2], dx
		mov register_table[4 * 2], si
		mov register_table[5 * 2], di
		
		mov ax, ds
		mov register_table[6 * 2], ax

		mov ax, es
		mov register_table[7 * 2], ax
		
		ret
endp

;---------------------------------------------------------------
; Draws one line of a frame
;
; Entry: BX - first symbol
;        AX - middle symbol (that is repeated)
;        DX - last symbol
;        DI - starting addr to draw
; Note:  ES - videoseg addr
; Exit:  DI - starting address of the next line, CX = 0, DF = 0
; Chngs: CX DI DF
;---------------------------------------------------------------
draw_frame_line proc
		cld
		
		mov cx, FRAME_LINE_LENGTH
		
		xchg bx, ax
		stosw
		xchg bx, ax
		dec cx
		
		dec cx            ; need to loop line length - 2 times
		rep stosw
		
		xchg dx, ax
		stosw
		xchg dx, ax
		
		ret
endp


;
; TODO.
;
draw_frame proc
		mov bx, '+'
		mov ax, '-'
		mov dx, '+'
		mov di, VIDEO_SEG
		call draw_frame_line
		
		ret
endp

;
; TODO.
;
draw_info proc
		call draw_frame
		
		ret
endp

;
; TODO.
;
int8_handler proc
		push ax bx cx dx di si es ds
		pushf
		
		mov ax, cs
		mov ds, ax

		call save_registers
		
		call draw_info
		
		popf
		pop ds es si di dx cx bx ax

		; iret
		db 0EAh                                     ; JMP FAR
int8_jmp_far_arg dw 2 dup(0)
endp

;
; TODO.
;
; Этот код будет гадить в регистры любой прерванной программе. В нашем случае ещё ничего, хотя мы al портим.
int9_handler proc
		push ax di es
		
		mov di, VIDEO_SEG
		mov es, di
		mov di, (5*80 + 80/2) * 2
		mov ah, 4eh

Next:
		in al, 60h
		stosw                       ; mov es:[di], ax
		                            ; and di, 0FFFh
		
		; Такой код неприлично писать без комментариев. Нужны комментарии.
		; Если scan код наш, то нужно будет говорить с клавиатурой и с контроллером прерываний, не прыгать на предыдущий обработчик.
	;	in al, 61h                  ; Send ACK to kbd
	;	mov ah, al
	;	or al, 80h
	;	out 61h, al
	;	mov al, ah
	;	out 61h, al
		
	;	mov al, 20h                 ; Send EOI to int cntrlr
	;	out 20h, al
		
		pop es di ax
		
		; iret
		db 0EAh                     ; JMP FAR
int9_jmp_far_arg dw 2 dup(0)
endp

VIDEO_SEG = 0b800h

; ax, bx, cx, dx, si, di, es, ds, fs, gs
NUM_REGISTERS = 8d
register_table dw NUM_REGISTERS dup(0)
registers db "ax", "bx", "cx", "dx", "si", "di", "es", "ds"

REGISTER_LENGTH = 2
VALUE_LENGTH = 2
FRAME_LINE_LENGTH = 1 + 1 + REGISTER_LENGTH + 1 + VALUE_LENGTH + 1 + 1

end_of_program:	
end start