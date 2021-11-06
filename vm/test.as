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
	jbe IF_2
IF_2:
	push 3
	push 1
	jae IF_2
JMP_TO_HALT:
	jmp HALT
