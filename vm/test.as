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
	jmp HALT
