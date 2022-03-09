.model tiny

.code
org 100h

locals

VIDEO_SEG = 0b800h

BORDER_VERT_PLAIN = 07bah
BORDER_HOR_PLAIN  = 07cdh
BORDER_TL_CORNER  = 07c9h
BORDER_TR_CORNER  = 07bbh
BORDER_BL_CORNER  = 07c8h
BORDER_BR_CORNER  = 07bch
BACKGROUND = 0700h

CHAR_COLOR = 0700h

SCREEN_ROWS = 25d
SCREEN_COLS = 80d

FRAME_ROWS = SCREEN_ROWS - 2d
FRAME_COLS = SCREEN_COLS - 2d

start:
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
; Draws frame
;
; Entry: None
; Note:  None
; Exit:  DF = 0
; Destr: AX BX CX DX SI DI ES DF
;---------------------------------------------------------------
draw_frame proc
		mov ax, VIDEO_SEG
		mov es, ax
		mov di, 0
	
		mov bx, BORDER_TL_CORNER
		mov ax, BORDER_HOR_PLAIN
		mov dx, BORDER_TR_CORNER
		mov cx, SCREEN_COLS
		call draw_line
	
		mov si, 1                          ; SI = current line index
		mov bx, BORDER_VERT_PLAIN
		mov ax, BACKGROUND
		mov dx, BORDER_VERT_PLAIN
@@loop:
		cmp si, SCREEN_ROWS - 1
		je @@loop_end
	
		mov cx, SCREEN_COLS
		call draw_line
	
		inc si
		jmp @@loop
@@loop_end:

		mov bx, BORDER_BL_CORNER
		mov ax, BORDER_HOR_PLAIN
		mov dx, BORDER_BR_CORNER
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

		rep movsw

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

hello_string db "For the End of the World spell, press Control-Alt-Delete. Хочешь устроить конец света? Нажми кнопку ресет. ", 0
second_string db "Если я ядовитый, то мне конец. Я язык прикусил. Нажмите любую клавишу, чтобы продолжить...", 0

end start