readf
pop ax

readf
pop bx

readf
pop cx

push ax
push 0
je LINEAR_EQUATION
jmp QUADRATIC_EQUATION

LINEAR_EQUATION:
	push bx
	push 0
	subf
	push EPS
	push -1.0
	mulf
	jbf B_IS_ZERO
	push cx
	push -1
	mul
	push bx
	div
	out
	halt
B_IS_ZERO:
	push NO_ROOTS_STR
	outs
	halt

QUADRATIC_EQUATION:
	push 4
	push ax
	push cx
	mul
	mul
	push bx
	push bx
	mul
	sub
	sqrt
	pop [SQRT_DESCRIMINANT]

	push ax
	push 2
	mul
	push [DESCRIMINANT]
	push bx
	push -1
	mul
	sub
	div
	out

	push [SPACE_CHARACTER]
	outb

	push ax
	push 2
	mul
	push [DESCRIMINANT]
	push bx
	push -1
	mul
	add
	div
	out
	halt

EPS:
	.db 0.05
SQRT_DESCRIMINANT:
	.db 0
SPACE_CHARACTER:
	.db " ", 0
NO_ROOTS_STR:
	.db "No roots.\0"