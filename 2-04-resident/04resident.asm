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

		mov bx, 9
		mov di, offset int9_jmp_far_arg
		mov si, offset int9_handler
		call set_int

		call go_resident

include ..\2-03-S~1\03.asm

;--------------------------------------------------------------------
; Go resident.
;
; Entry: none
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
;        DI    - address in ds segment to store offset and segment
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

;--------------------------------------------------------------------
; Restore previously saved value of interrupt table entry
;
; Entry: BX    - interrupt number
;        DI    - address in ds segment of the stored offset and segment
; Note:  none
; Exit:  ES = 0
; Chngs: AX BX ES
;--------------------------------------------------------------------
restore_int proc
		cli
		
		xor ax, ax
		mov es, ax
		
		shl bx, 2                           ; multiply bx by 4 so that it is address of the offset.
		mov ax, [di]
		mov es:[bx], ax
		mov ax, [di+2]
		mov es:[bx+2], ax

		sti
		
		ret
endp


;--------------------------------------------------------------------
; Saves register values to display them later.
;
; Entry: none
; Note:  none
; Exit:  none
; Chngs: AX
;--------------------------------------------------------------------
save_registers proc
		mov cs:register_table[0 * 2], ax
		mov cs:register_table[1 * 2], bx
		mov cs:register_table[2 * 2], cx
		mov cs:register_table[3 * 2], dx
		mov cs:register_table[4 * 2], si
		mov cs:register_table[5 * 2], di
		
		mov ax, ds
		mov cs:register_table[6 * 2], ax

		mov ax, es
		mov cs:register_table[7 * 2], ax
		
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
		
		mov cx, FRAME_COLS
		
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


;---------------------------------------------------------------
; Draws frame.
;
; Entry: none
; Note:  ES - videoseg addr
; Exit:  none
; Chngs: AX BX DX SI DI
;---------------------------------------------------------------
draw_frame proc
		mov si, current_frame_style
		
		; Set foreground and background.
		mov bh, 7
		mov ah, bh
		mov dh, ah

		mov bl, [si + BORDER_TL_CORNER_OFFSET]
		mov al, [si + BORDER_HOR_PLAIN_OFFSET]
		mov dl, [si + BORDER_TR_CORNER_OFFSET]

		mov di, 0
		call draw_frame_line
				
		mov bl, [si + BORDER_VERT_PLAIN_OFFSET]
		mov al, ' '
		mov dl, [si + BORDER_VERT_PLAIN_OFFSET]

		mov si, FRAME_ROWS - 2
@@loop:
		test si, si
		jz @@loop_end
		
		add di, 2 * (SCREEN_COLS - FRAME_COLS)
				
		call draw_frame_line
		
		dec si
		
		jmp @@loop
@@loop_end:
		
		add di, 2 * (SCREEN_COLS - FRAME_COLS)

		mov si, current_frame_style
		mov bl, [si + BORDER_BL_CORNER_OFFSET]
		mov al, [si + BORDER_HOR_PLAIN_OFFSET]
		mov dl, [si + BORDER_BR_CORNER_OFFSET]

		call draw_frame_line

		ret
endp

;---------------------------------------------------------------
; Displays string.
;
; Entry: DS:SI - source string
;        ES:DI - dest address in video memory
; Note:  ES - videoseg addr
; Exit:  none
; Chngs: none
;---------------------------------------------------------------
display_string proc
@@copy_loop:
		lodsb
		test al, al
		jz @@copy_loop_end

		stosw
		jmp @@copy_loop
@@copy_loop_end:

		ret
endp

;---------------------------------------------------------------
; Pad number so that it has the specified length.
;
; Entry: DS:SI - address of the string
; Note:  none
; Exit:  SI - new start of the string
; Chngs: SI
;---------------------------------------------------------------
pad_number proc
		cld
		
		push cx
		push es
		push ax
		
		mov ax, ds
		mov es, ax
		
		pop ax
		
		xchg si, di
		
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
		push di
		call strlen
		pop di
		
		xchg si, di
		
		pop es
				
		xor cx, 0FFFFh
		add cx, 1
		
		add cx, 4

		sub si, cx
		
		pop cx
		
		ret
endp

;---------------------------------------------------------------
; Draw labels.
;
; Entry: none
; Note:  none
; Exit:  none
; Chngs: none
;---------------------------------------------------------------
draw_labels proc
		cld
		
		push ax
		push es
		push si
		push cx
		push di

		mov di, (SCREEN_COLS * 2 + (1 + 1) * 2)
		mov @@cur_line_start, di
		xor cx, cx
				
		mov ax, VIDEO_SEG
		mov es, ax
		
		mov ah, 7h
		mov si, offset registers

@@loop:
		cmp cx, NUM_REGISTERS
		je @@loop_end
		
		lodsb
		stosw
		
		lodsb
		stosw
		
		mov di, @@cur_line_start
		add di, SCREEN_COLS * 2
		mov @@cur_line_start, di
		
		
		inc cx
		jmp @@loop

@@loop_end:
		pop di
		pop cx
		pop si
		pop es
		pop ax

		ret

@@cur_line_start dw 0
endp


;---------------------------------------------------------------
; Draws register values.
;
; Entry: none
; Note:  none
; Exit:  none
; Chngs: AX CX DI ES DF
;---------------------------------------------------------------
draw_values proc
		cld
		
		mov di, (SCREEN_COLS * 2 + (1 + 1 + 2 + 1) * 2)
		mov @@cur_line_start, di
		xor cx, cx
		
@@loop:
		cmp cx, NUM_REGISTERS
		je @@loop_end
		
		push cx                          ; TODO: TRY to avoid.
		push di
		
		mov si, cx
		shl si, 1
		mov ax, register_table[si]
		shr si, 1
		mov bx, 16
		mov di, ds
		mov es, di
		mov di, offset @@number_as_str
		call itoa
		
		pop di
		pop cx
		
		mov ax, VIDEO_SEG
		mov es, ax
		
		mov ah, 7h
		mov si, offset @@number_as_str
		
		call pad_number                    ; SI = padded number
		
		call display_string
		
		mov di, @@cur_line_start
		add di, SCREEN_COLS * 2
		mov @@cur_line_start, di
		
		
		inc cx
		jmp @@loop

@@loop_end:
		ret

@@cur_line_start dw 0
@@number_padding db 4 dup ('0')
@@number_as_str db 6 dup (0)
endp

;---------------------------------------------------------------
; Draw information.
;
; Entry: none
; Note:  none
; Exit:  none
; Chngs: AX BX CX DX SI DI ES DS
;---------------------------------------------------------------
draw_info proc
		mov ax, VIDEO_SEG
		mov es, ax
		call draw_frame
		
		call draw_labels
		
		call draw_values
		
		ret
endp

;---------------------------------------------------------------
; The new handler for interrupt 8.
;
; Entry: none
; Note:  none
; Exit:  none
; Chngs: AX BX DX SI DI
;---------------------------------------------------------------
int8_handler proc
		push ax bx cx dx di si es ds
		pushf
		
		call save_registers
		
		push ax
		mov ax, cs
		mov ds, ax
		pop ax

		call draw_info
				
		popf
		pop ds es si di dx cx bx ax

		; iret
		db 0EAh                                     ; JMP FAR
int8_jmp_far_arg dw 2 dup(0)
endp

;---------------------------------------------------------------
; The new handler for interrupt 9.
;
; Entry: none
; Note:  none
; Exit:  none
; Chngs: AX BX DX SI DI
;---------------------------------------------------------------
; F1+Z --- выйти, F1+X --- показать/скрыть, F1+Space --- переключить стиль.
int9_handler proc
		push ax di ds bx
		
		mov ax, cs
		mov ds, ax
		
		in al, 60h
		
		mov ah, al
		mov bl, KEY_RELEASED_MASK
		and ah, bl
		
		mov bl, (KEY_RELEASED_MASK xor 0FFFFh)    ; store inversed bits of the mask
		and al, bl
		
		cmp al, F1_SCAN_CODE
		je @@process_f1
		
		cmp al, Z_SCAN_CODE
		je @@process_z
		
		cmp al, X_SCAN_CODE
		; je @@process_x
		
		cmp al, SPACE_SCAN_CODE
		je @@process_space
		
		jmp @@pass_to_prev_int9
		
@@process_f1:
		test ah, ah
		jz @@f1_pressed
		jnz @@f1_released

@@f1_pressed:
		mov f1_pressed, 1
		jmp @@consume_key

@@f1_released:
		mov f1_pressed, 0
		jmp @@consume_key

@@process_z:
		test ah, ah
		jnz @@consume_key           ; Process only key press, not key release.
		
		mov al, f1_pressed
		test al, al
		jz @@pass_to_prev_int9
		
		push es
		
		mov bx, 9
		mov di, offset int9_jmp_far_arg
		call restore_int
		
		mov bx, 8
		mov di, offset int8_jmp_far_arg
		call restore_int
		
		pop es
		
		; TODO: restore old picture.
		
		jmp @@consume_key

@@process_x:
		test ah, ah
		jnz @@consume_key           ; Process only key press, not key release.
		
		mov al, f1_pressed
		test al, al
		jz @@consume_key
		jmp @@pass_to_prev_int9

@@process_space:
		test ah, ah
		jnz @@consume_key           ; Process only key press, not key release.

		mov al, f1_pressed
		test al, al
		jz @@pass_to_prev_int9

		mov ax, current_frame_style
		add ax, STYLE_ENTRY_SIZE
		cmp ax, offset style_table_end
		jne @@skip_cycling_style
		
		mov ax, offset style_table
@@skip_cycling_style:

		mov current_frame_style, ax

@@consume_key:
		; Такой код неприлично писать без комментариев. Нужны комментарии.
		; Если scan код наш, то нужно будет говорить с клавиатурой и с контроллером прерываний, не прыгать на предыдущий обработчик.
		in al, 61h                  ; Send ACK to kbd
		mov ah, al
		or al, 80h
		out 61h, al
		mov al, ah
		out 61h, al
		
		mov al, 20h                 ; Send EOI to int cntrlr
		out 20h, al

		pop bx ds di ax
		iret
		
@@pass_to_prev_int9:
		pop bx ds di ax
		
		db JMP_FAR_OPCODE
int9_jmp_far_arg dw 2 dup(0)
endp

JMP_FAR_OPCODE = 0eah

VIDEO_SEG = 0b800h

f1_pressed db 0
z_pressed db 0
x_pressed db 0
space_pressed db 0

KEY_RELEASED_MASK = 80h

F1_SCAN_CODE = 3bh
Z_SCAN_CODE = 2ch
X_SCAN_CODE = 2dh
SPACE_SCAN_CODE = 39h

; ax, bx, cx, dx, si, di, es, ds, fs, gs
NUM_REGISTERS = 8d
register_table dw NUM_REGISTERS dup(0)
registers db "ax", "bx", "cx", "dx", "si", "di", "ds", "es"

REGISTER_LENGTH = 2
VALUE_LENGTH = 4
FRAME_COLS = 1 + 1 + REGISTER_LENGTH + 1 + VALUE_LENGTH + 1 + 1
FRAME_ROWS = 1 + NUM_REGISTERS + 1
SCREEN_COLS = 80

BORDER_VERT_PLAIN_OFFSET = 0d
BORDER_HOR_PLAIN_OFFSET  = 1d
BORDER_TL_CORNER_OFFSET  = 2d
BORDER_TR_CORNER_OFFSET  = 3d
BORDER_BL_CORNER_OFFSET  = 4d
BORDER_BR_CORNER_OFFSET  = 5d

style_table:
STYLE_ENTRY_SIZE = 1d * 6d
; vertical plain border, horizonal plain border, top left corner, top right corner, bottom left corner, bottom right
; user_frame        db 0,       0,    0,    0,    0,     0
single_line_frame db 0b3h, 0c4h, 0dah, 0bfh, 0c0h,  0d9h
double_line_frame db 0bah, 0cdh, 0c9h, 0bbh, 0c8h,  0bch
plus_frame        db 0b3h, 0c4h, 0c5h, 0c5h, 0c5h,  0c5h
style_table_end:

; address of the table entry for the current style
current_frame_style dw offset style_table

end_of_program:	
end start