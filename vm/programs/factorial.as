read
pop ax

push 1
pop bx

FACTORIAL:
	push 1
	push ax
	jbe FACTORIAL_EXIT
	push ax
	push bx
	mul
	pop bx
	push 1
	push ax
	sub
	pop ax
	jmp FACTORIAL


FACTORIAL_EXIT:
	push bx
	out
	trap
	halt
