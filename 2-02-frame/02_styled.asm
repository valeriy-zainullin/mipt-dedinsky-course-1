.model tiny

.code
org 100h

locals

VIDEO_SEG = 0b800h

CHAR_COLOR = 07h

SCREEN_ROWS = 25d
SCREEN_COLS = 80d

FRAME_ROWS = SCREEN_ROWS - 2d
FRAME_COLS = SCREEN_COLS - 2d

STYLE_INDEX = 2d

ARG_LENGTH_ADDR = 80h
ARG_STRING_ADDR = 82h

start:
		call parse_args
		
		mov ax, VIDEO_SEG
		mov es, ax
		
		call draw_frame
		
		mov bx, offset hello_string
		call send_string

		mov bx, offset second_string
		call send_string

		xor ah, ah
		int 16h

		mov ax, 4c00h                            ; Выйти из программы и передать управление DOSу
		int 21h

;---------------------------------------------------------------
; Parses command line arguments
;
; Entry: None
; Note:  None
; Exit:  style - the requested style or 0 if no style was requested
; Destr: AX BX CX SI ES
;---------------------------------------------------------------
parse_args proc
		cld
		
		mov si, ARG_STRING_ADDR
		mov cx, ds:[ARG_LENGTH_ADDR]
		
		call skip_spaces
		
		lodsb
		
		sub al, '0'
		
		cmp al, 0
		jl @@wrong_style
		je @@read_user_style
		cmp al, 4	
		jge @@wrong_style
		
		xor bx, bx
		mov bl, STYLE_ENTRY_SIZE
		mul bl
		add ax, offset style_table
		mov current_frame_style, ax

		ret

@@wrong_style:
		mov current_frame_style, offset single_line_frame
		ret

@@read_user_style:
		cmp cx, 0
		je @@wrong_style
		
		mov dx, ds
		mov es, dx
		mov di, offset user_frame
		
		xor dx, dx
		; 6 times
@@loop:
		cmp dx, 6
		je @@loop_end

		call skip_spaces
		
		cmp cx, 0
		je @@wrong_style
				
		movsb
		
		inc dx
		jmp @@loop
@@loop_end:
		
		mov current_frame_style, offset user_frame
		ret
endp

;---------------------------------------------------------------
; Skips space characters
;
; Entry: DS:SI - address of the string to skip characters in
;        CX - length
; Note:  None
; Exit:  CX - length of the remaining string portion
;        DS:SI - non space character encountered or the last
;                character of the string
; Destr: AX CX SI
;---------------------------------------------------------------
skip_spaces proc
		cld
		
		cmp cx, 0
		je @@exit
		
		cmp byte ptr [si], ' '
		jne @@exit
		
		mov ax, ds
		push es
		mov es, ax
		
		xchg si, di
		
		mov al, ' '
		
		repe scasb
		
		dec di
		
		xchg si, di
		pop es
@@exit:
		ret
endp

;---------------------------------------------------------------
; Draws frame
;
; Entry: None
; Note:  None
; Exit:  DF = 0
; Destr: AX BX CX DX SI DI BP ES DF
;---------------------------------------------------------------
draw_frame proc
		mov ax, VIDEO_SEG
		mov es, ax
		mov di, 0
	
		mov ah, CHAR_COLOR
		mov bh, CHAR_COLOR
		mov dh, CHAR_COLOR

		mov bp, current_frame_style
		mov bl, [bp+BORDER_TL_CORNER_OFFSET]
		mov al, [bp+BORDER_HOR_PLAIN_OFFSET]
		mov dl, [bp+BORDER_TR_CORNER_OFFSET]
		mov cx, SCREEN_COLS
		call draw_line
	
		mov si, 1                                  ; SI = current line index
		mov bl, [bp+BORDER_VERT_PLAIN_OFFSET]
		mov al, 0
		mov dl, [bp+BORDER_VERT_PLAIN_OFFSET]
@@loop:
		cmp si, SCREEN_ROWS - 1
		je @@loop_end
	
		mov cx, SCREEN_COLS
		call draw_line
	
		inc si
		jmp @@loop
@@loop_end:

		mov bl, [bp+BORDER_BL_CORNER_OFFSET]
		mov al, [bp+BORDER_HOR_PLAIN_OFFSET]
		mov dl, [bp+BORDER_BR_CORNER_OFFSET]
		mov cx, SCREEN_COLS
		call draw_line
	
		ret
endp

;---------------------------------------------------------------
; Draws one line of a frame
;
; Entry: BX - first symbol
;        AX - middle symbol (that is repeated)
;        DX - last symbol
;        CX - line length (frame width)
;        DI - starting addr to draw
; Note:  ES - videoseg addr
; Exit:  DI - starting address of the next line, CX = 0, DF = 0
; Chngs: CX DI DF
;---------------------------------------------------------------
draw_line proc
		cld
		
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

; TODO: DF = 0, cld

;--------------------------------------------------------------------
; Shifts line inside the frame to the left for one character.
;
; Entry: CX - length of the line inside the frame (# screen cols - 2)
;        DI - address of the first character inside the frame on the line
; Note:  DS, ES - videoseg addr
; Exit:  DI - address of the last character inside the frame on the line
;        SI - address of the last frame border character on the line
;        CX = 0
; Chngs: CX SI DI DF
;--------------------------------------------------------------------
shift_line proc
		cld
		
		mov si, di
		add si, 2         ; SI = address of the character after
		                  ;      the current
		dec cx            ; need to copy # cols - 1 chars

if ANIMATED_SHIFT
else
		rep movsw
endif

		ret
endp

;---------------------------------------------------------------
; Shift chars inside the frame to make space for a new one
;
; Entry: None
; Note:  ES, DS - videoseg addr
; Exit:  CX = 0, DF = 0
; Chngs: AX CX SI DI DF
;---------------------------------------------------------------
shift_chars proc
		mov ax, 0                     ; AX = index of the current
		                              ;      line inside the frame
		mov di, 2d * SCREEN_COLS + 2d ; DI = address of the current
		                              ;      character in video memory
		                              ; Initially of the first character
		                              ; inside the frame
@@loop:
		cmp ax, FRAME_ROWS - 1   ; do unless it is the last line inside the frame
		je @@loop_end
		
		; Shifts line inside the frame to the left for one character.
		;
		; Entry: CX - length of the line inside the frame (# screen cols - 2)
		;        DI - address of the first character inside the frame on the line
		; Note:  DS, ES - videoseg addr
		; Exit:  DI - address of the last character inside the frame on the line
		;        SI - address of the last frame border character on the line
		;        CX = 0
		; Chngs: CX SI DI
		mov cx, FRAME_COLS
		call shift_line
		
		add si, 4d               ; point to the first character inside the frame
		                         ; on the next line

		movsw                    ; shift the last character of the line
		
		add di, 4d               ; skip borders of the frame
		
		inc ax
		jmp @@loop
@@loop_end:

		mov cx, FRAME_COLS
		call shift_line          ; shift the last line without filling the last character

		ret
endp

;---------------------------------------------------------------
; TODO: update this doc.
; Sends char to frame
;
; Entry: AL - character to send
; Note:  ES - videoseg addr
; Exit:  None
; Destr: CX
;---------------------------------------------------------------
send_char proc
		cmp @@current_col, SCREEN_COLS - 1
		jne @@shift_chars_end
		
		dec @@current_col

		push ax
		; Shift chars inside the frame to make space for a new one
		;
		; Entry:
		; Note:  ES, DS - videoseg addr
		; Exit:  CX = 0
		; Chngs: AX CX SI DI
		push ds
		mov ax, es
		mov ds, ax
		call shift_chars
		pop ds
		pop ax
@@shift_chars_end:
	
		mov bx, @@current_col
		shl bx, 1
		mov es:[2d * (SCREEN_ROWS - 2d) * SCREEN_COLS + bx], al
		shr bx, 1
		
		inc @@current_col

		ret

@@current_col dw 1d
endp

send_string proc
@@loop:
		mov al, [bx]
		cmp al, 0
		je @@loop_end

		push bx
		call send_char
		pop bx

		inc bx
		jmp @@loop
@@loop_end:
		ret
endp

.data

BORDER_VERT_PLAIN_OFFSET = 0d
BORDER_HOR_PLAIN_OFFSET  = 1d
BORDER_TL_CORNER_OFFSET  = 2d
BORDER_TR_CORNER_OFFSET  = 3d
BORDER_BL_CORNER_OFFSET  = 4d
BORDER_BR_CORNER_OFFSET  = 5d

style_table:
STYLE_ENTRY_SIZE = 1d * 6d
; vertical plain border, horizonal plain border, top left corner, top right corner, bottom left corner, bottom right
user_frame        db 0,       0,    0,    0,    0,     0
single_line_frame db 0b3h, 0c4h, 0dah, 0bfh, 0c0h,  0d9h
double_line_frame db 0bah, 0cdh, 0c9h, 0bbh, 0c8h,  0bch
plus_frame        db 0b3h, 0c4h, 0c5h, 0c5h, 0c5h,  0c5h

; address of the table entry for the current style
current_frame_style dw 0h

hello_string db "For the End of the World spell, press Control-Alt-Delete. Хочешь устроить конец света? Нажми кнопку ресет. ", 0
second_string db "Если я ядовитый, то мне конец. Я язык прикусил. Нажмите любую клавишу, чтобы продолжить...", 0

end start