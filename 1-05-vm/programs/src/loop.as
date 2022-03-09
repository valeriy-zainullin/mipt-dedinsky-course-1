	push 0
	pop ax

LOOP_START:
	push ax
	push 5
	jaei LOOP_EXIT

	push ax
	writei

	push ax
	push 1
	addi
	pop ax

	jmp LOOP_START

LOOP_EXIT:
	halt
