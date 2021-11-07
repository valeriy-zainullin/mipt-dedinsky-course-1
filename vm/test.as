push 1
pop ax
push 2
pop bx
push 3
pop cx
push ax
push bx
push cx
add
add
add

out:
	jmp DO_MORE

HALT:
	halt

DO_MORE:
	out
	push 1
	push 2
	sub
	outb
	push 1
	push 1
	jae IF_1
IF_1:
	push 2
	push 2
	push BEFORE_JMP_TO_HALT
	call IF_2
IF_2:
	push 3
	push 1
	jae IF_2
	ret
BEFORE_JMP_TO_HALT:
	push OUTPUT_STRING
	call SEND_STRING
	push OUTPUT_NUMBER
	call SEND_STRING
JMP_TO_HALT:
	jmp HALT

SEND_STRING:
	pop bx
	pop ax
	push bx
SEND_STRING_LOOP:
	push [ax]
	push 255
	and
	push 0
	je SEND_STRING_EXIT
	push [ax]
	push 255
	and
	outb
	push ax
	push 1
	add
	pop ax
	jmp SEND_STRING_LOOP
SEND_STRING_EXIT:
	ret

OUTPUT_STRING:
	.db "123, abacaba\n\0", 0, 1234
OUTPUT_NUMBER:
	.db 1234
