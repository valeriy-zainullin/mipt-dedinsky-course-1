readi
pop ax

push 1
pop bx

FACTORIAL:
	push 1
	push ax
	jbei FACTORIAL_EXIT
	push ax
	push bx
	muli
	pop bx
	push 1
	push ax
	subi
	pop ax
	jmp FACTORIAL


FACTORIAL_EXIT:
	push bx
	writei
	trap
	halt
