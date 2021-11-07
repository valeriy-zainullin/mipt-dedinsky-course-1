	push 0
	pop ax

LOOP_START:
	push ax
	push 5
	je LOOP_EXIT

	push ax
	out

	push ax
	push 1
	add
	pop ax

	jmp LOOP_START

LOOP_EXIT:
	halt
