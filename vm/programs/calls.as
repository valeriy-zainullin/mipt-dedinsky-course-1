	push FREE_MEMORY
	pop cx

	push 0
	pop ax

LOOP_START:
	push ax
	push 5
	je LOOP_EXIT

	push ax
	call WRITE_NUMBER

	push ax
	push 1
	add
	pop ax

	jmp LOOP_START

LOOP_EXIT:
	halt

WRITE_NUMBER:
	pop [cx]
	pop [cx+4]
	push cx

	push cx
	push 8
	add
	pop cx

	push [cx+-4]
	out

	pop cx
	push [cx]
	out
	push [cx]
	ret

FREE_MEMORY:
