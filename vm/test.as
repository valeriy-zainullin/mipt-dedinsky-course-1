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
	outs
	push OUTPUT_NUMBER
	out
JMP_TO_HALT:
	jmp HALT

SEND_STRING:
	pop ax
SEND_STRING_LOOP:
	push [ax]
	and 255
	push 0
	je SEND_STRING_EXIT
	push [ax]
	and 255
	out
	push ax
	push 1
	add
	pop ax
	jmp SEND_STRING_LOOP

OUTPUT_STRING:
	.db "123, abacaba\n\0"
OUTPUT_NUMBER:
	.db 1234
