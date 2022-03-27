jmp AFTER_VARS
A:
	.db 0
B:
	.db 0
C:
	.db 0
; EPS: .db 0.000001
AFTER_VARS:

jmp AFTER_PUSH_EPS
PUSH_EPS:
	pop ax
;	push 0.000001
	push [EPS]
	push ax
	ret
AFTER_PUSH_EPS:

readf
pop [A]

readf
pop [B]

readf
pop [C]

push [A]
push 0
call ABS
call PUSH_EPS
jbef LINEAR_EQUATION
jmp QUADRATIC_EQUATION

ABS:
	repush 1
	push 0
	jaef ABS_POS
ABS_NEG:
	push -1.0
	mulf
ABS_POS:
	pop ax
	pop bx
	push ax
	push bx
	ret


LINEAR_EQUATION:
	push [B]
	push 0
	subf
	call ABS
	call PUSH_EPS
	jbef B_IS_ZERO
	push [C]
	push -1.0
	mulf
	push [B]
	divf
	writef
	halt
B_IS_ZERO:
DESCRIMINANT_IS_ZERO:
NO_ROOTS:
	push NO_ROOTS_STR
	call WRITE_STRING
	halt

QUADRATIC_EQUATION:
	push 4.0
	push [A]
	push [C]
	mulf
	mulf
	push [B]
	push [B]
	mulf
	subf
	pop [DESCRIMINANT]
	
	call PUSH_EPS
	push [DESCRIMINANT]
	jbef DESCRIMINANT_IS_ZERO

	push [A]
	push 2.0
	mulf
	push [DESCRIMINANT]
	sqrtf
	push [B]
	push -1.0
	mulf
	subf
	divf
	writef

	push [SPACE_CHARACTER]
	writeb

	push [A]
	push 2.0
	mulf
	push [DESCRIMINANT]
	sqrtf
	push [B]
	push -1.0
	mulf
	addf
	divf
	writef

	halt

WRITE_STRING:
	pop ax
WRITE_STRING_LOOP:
	push 0
	je WRITE_STRING_END
	
	push [ax]
	writeb

	push 1
	push ax
	addi
	pop ax
	
	jmp WRITE_STRING_LOOP
	
WRITE_STRING_END:
	ret

EPS:
	.db 0.05
SQRT_DESCRIMINANT:
	.db 0
SPACE_CHARACTER:
	.db " ", 0
NO_ROOTS_STR:
	.db "No roots.",0
