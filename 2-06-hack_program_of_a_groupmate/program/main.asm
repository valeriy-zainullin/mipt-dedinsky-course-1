%include "../../2-05-printf/winapi.inc"

; TODO: удалить отладочную информацию.
; 2022.04.08: если раскомментировать код, то будут подключаться crt o-ошники при линковке,
; -nostdlib это не отключает. Круто. Есть ещё GlobalAlloc и GlobalFree, но они что-то не
; работают (может, я неправильно использовал). В любом случае рекомендую использовать
; HeapAlloc и HeapFree, они новее. Коммит 3103064e29f3e7fcc574e6040c6ebde66c430e71, можно
; задебажить, зарепортить и предложить решение.
; Сделал свою библиотеку mykernel32, которая просто выставляет эти функции, а выходная dll
; называется kernel32.dll. Обычно на windows импорт dll по имени файла, потому .a для этой
; библиотеки будут добавлять поля в таблицу импорта. С -lkernel32, при использовании
; встроенной библиотеки mingw, компилируется, что хороший знак для меня, но библиотека в
; mingw устарела, она импортирует BaseCheckRunApp, которого в новых версиях windows
; (windows 10) нет.

; 2022.04.09: хранить пароль в исполняемом файле без кодирования плохо, я решил хранить хеш.
; Хотел использовать библиотеки windows для хеширования, но на ассемблере у меня постоянно
; возникают проблемы: сначала create_sha512_hash, там BCryptOpenAlgorithmProvider не
; возвращается, хотя у меня всё правильно, я пользовался примером с сайта Майкрософт,
; переписал на ассемблер, всё четко проверил. Затем я просто взял пример на C от Майкрсофт,
; переделал, получил create_sha512_hash2. Тогда возникают проблемы на ExitProcess, там
; выгружаются dll-ки bcrypt, и в одной из них segmentation fault происходит.
; Может быть, потом отбебажу create_sha512_hash.
; Сейчас просто взял код из github-а sha512 под MIT лицензией, он в папке
; LeFroid_sha256-512-master, немного его подправил под себя, ссылка на оригинал в source.txt.

global __start

extern GetStdHandle
extern ReadFile
extern SetConsoleOutputCP
extern SetConsoleMode
extern GetProcessHeap
extern HeapAlloc
extern HeapFree
extern ExitProcess

extern IsDebuggerPresent
extern CheckRemoteDebuggerPresent


extern myprintf
extern mystrlen


extern BCryptOpenAlgorithmProvider
extern BCryptGetProperty
extern BCryptCreateHash
extern BCryptHashData
extern BCryptFinishHash
extern BCryptCloseAlgorithmProvider
extern BCryptDestroyHash

extern SHA512Hash

CODE_PAGE equ 65001

section .text
validate_handle:
		cmp rax, INVALID_HANDLE_VALUE
		xor rcx, rcx
		sub rsp, 32
		je ExitProcess
		add rsp, 32
		ret
		
section .text
__start:
		mov rcx, CODE_PAGE
		sub rsp, 32
		call SetConsoleOutputCP
		add rsp, 32
		
		test rax, rax
		jz .set_console_cp_failed
				
		call main

		mov rcx, rax
		sub rsp, 32
		call ExitProcess
		add rsp, 32

.set_console_cp_failed:
		mov rcx, 3
		sub rsp, 32
		call ExitProcess
		
section .text
; 0 -- undefined behaviour (it will fill all memory starting from rcx with dl.)
mymemset:
		push rdi
		push rax

		mov rdi, rcx
		mov rax, rdx
		mov rcx, r8
		rep stosb
		
		pop rax
		pop rdi
		ret
		
.on_different_lengths:
		mov rax, r8
		sub rax, r9
		
		pop rsi
		pop rdi
		ret

section .text
create_sha512_hash:
		push rcx                      ; buffer address
		push rdx                      ; size of buffer

		sub rsp, 32
		call GetProcessHeap
		add rsp, 32
		
		test rax, rax
		jz .hashing_failed
		
		mov [rel process_heap], rax

		lea rcx, [rel algo_handle]
		lea rdx, [rel sha512_alg_prop]
		xor r8, r8
		xor r9, r9
		sub rsp, 32
		call BCryptOpenAlgorithmProvider
		add rsp, 32
		
		; Negative dword means an error for NTSTATUS.
		test eax, eax
		js .hashing_failed
		
		; Getting has object size
		mov qword [rel hash_object_size], 0
		mov rcx, [rel algo_handle]
		lea rdx, [rel hash_object_size_prop]
		lea r8, [rel hash_object_size]
		mov r9, 4
		lea rax, [rel num_bytes_copied]
		push 0
		push rax
		sub rsp, 32
		call BCryptGetProperty
		add rsp, 32
		add rsp, 16
		
		; Negative dword means an error for NTSTATUS.
		test eax, eax
		js .hashing_failed
		
		mov rcx, [rel process_heap]
		xor rdx, rdx
		mov r8, [rel hash_object_size]
		sub rsp, 32
		call HeapAlloc
		add rsp, 32
		
		; Negative dword means an error for NTSTATUS.
		test eax, eax
		js .hashing_failed
		
		mov [rel hash_object_ptr], rax

		mov dword [rel hash_str_len], 0
		mov rcx, [rel algo_handle]
		lea rdx, [rel hash_len_prop]
		lea r8, [rel hash_str_len]
		mov r9, 4
		lea rax, [rel num_bytes_copied]
		push 0
		push rax
		sub rsp, 32
		call BCryptGetProperty
		add rsp, 32
		add rsp, 16
		
		; Negative dword means an error for NTSTATUS.
		test eax, eax
		js .hashing_failed
		
		cmp dword [rel hash_str_len], EXPECTED_HASH_LEN
		jne .hashing_failed
		
		mov rcx, [rel process_heap]
		xor rdx, rdx
		mov r8, [rel hash_str_len]
		sub rsp, 32
		call HeapAlloc
		add rsp, 32
		
		; Negative dword means an error for NTSTATUS.
		test eax, eax
		js .hashing_failed
		
		mov [rel hash_str_ptr], rax
		
		mov rcx, [rel algo_handle]
		lea rdx, [rel hashing_handle]
		mov r8, [rel hash_object_ptr]
		mov r9, [rel hash_object_size]
		push 0
		push 0
		push 0
		sub rsp, 32
		call BCryptCreateHash
		add rsp, 32
		add rsp, 24
		
		; Negative dword means an error for NTSTATUS.
		test eax, eax
		js .hashing_failed

		mov rcx, [rel hashing_handle]
		mov rdx, [rsp+8]              ; address of the buffer
		mov r8, [rsp]                 ; size of the buffer
		xor r9, r9
		sub rsp, 32
		call BCryptHashData
		add rsp, 32
		
		; Negative dword means an error for NTSTATUS.
		test eax, eax
		js .hashing_failed
		
		mov rcx, [rel hashing_handle]
		mov rdx, [rel hash_str_ptr]
		mov r8, [rel hash_str_len]
		xor r9, r9
		sub rsp, 32
		call BCryptFinishHash
		add rsp, 32
		
		; Negative dword means an error for NTSTATUS.
		test eax, eax
		js .hashing_failed

		
		mov rcx, [rel algo_handle]
		sub rsp, 32
		call BCryptCloseAlgorithmProvider
		add rsp, 32
		
		mov rcx, [rel hashing_handle]
		sub rsp, 32
		call BCryptDestroyHash
		add rsp, 32
		
		mov rcx, [rel process_heap]
		xor rdx, rdx
		mov r8, [rel hash_object_ptr]
		sub rsp, 32
		call HeapFree
		add rsp, 32
		
		mov rax, [rel hash_str_ptr]

		pop rdx
		pop rcx
		ret
		
.hashing_failed:
		mov rax, 1
		sub rsp, 32
		call ExitProcess
		
section .text
delete_sha512_hash:
		mov rdx, rcx
		mov rcx, [rel process_heap]
		mov r8, 0
		sub rsp, 32
		call HeapFree
		add rsp, 32
		ret
		
section .bss
algo_handle: resq 1
process_heap: resq 1
hashing_handle: resq 1

hash_object_size: resq 1
hash_str_len: resd 1

hash_str_ptr: resq 1
hash_object_ptr: resq 1

; To squelch BCryptGetProperty.
; It needs to output number of bytes written.
; Otherwise reports an error.
; TODO: check!
num_bytes_copied: resq 1

section .rdata
%macro wchar_str 1
	%assign i 1
	%strlen len %1
	%rep len
		%substr char %1 i
		db char, 0
		%assign i i+1
	%endrep
	db 0, 0
	align 8, db 0
%endmacro
; wchar_t strings.
; ntdll's Rtl functions use SSE! We must align all of the variables to 16 bytes boundary. Hell!!
sha512_alg_prop: wchar_str "SHA512"
hash_object_size_prop: wchar_str "ObjectLength"
hash_len_prop: wchar_str "HashDigestLength"

section .text
cmp_counted_strs:
		push rdi
		push rsi
		
		mov rsi, rcx
		mov rdi, rdx
		mov rcx, r8
		
		repe cmpsb
		dec rsi
		dec rdi
		
		movzx rcx, byte [rsi]
		movzx rdx, byte [rdi]
		sub rcx, rdx
		
		pop rsi
		pop rdi
		
		mov rax, rcx
		ret


ENABLE_LINE_INPUT equ 0x2
section .text
main:
		lea rcx, [rel greeting_fmt]
		mov rdx, [rsp]
		sub rsp, 32
		call myprintf
		add rsp, 32
		
		mov rcx, STD_INPUT_HANDLE
		sub rsp, 32
		call GetStdHandle
		add rsp, 32
		mov [rel stdin_handle], rax
		call validate_handle
		
		mov rbp, rsp
		and rsp, ~0xF               ; Align to 16 bytes as dlls and the kernel may use SSE.
		sub rsp, BUFFER_SIZE
		mov [rel buffer_ptr], rsp
		
		mov rcx, [rel buffer_ptr]
		xor dl, dl
		mov r8, BUFFER_SIZE
		sub rsp, 32
		call mymemset
		add rsp, 32
		
		mov rcx, [rel stdin_handle]
		mov rdx, [rel buffer_ptr]
		mov r8, BUFFER_SIZE - 1 ; + BUFFER_OVERRUN
		xor r9, r9
		push 0
		sub rsp, 32
		call ReadFile
		add rsp, 32
		add rsp, 8
		
		;--------------------------------------------------------------------
		; Returns length of a null terminated string.
		;
		; Entry: RCX - address of the start of the string
		; Note:  max string length is 2^64 - 1.
		; Exit:  RAX - length of the string
		; Chngs: RAX RCX
		;--------------------------------------------------------------------
		; size_t strlen( const char * string );
		mov rcx, [rel buffer_ptr]
		; sub rsp, 32  ; not needed for my strlen, as it is not used anyway.
		call mystrlen
		; add rsp, 32

		mov rcx, [rel buffer_ptr]
		mov rdx, rax
		; mov rdx, BUFFER_SIZE
		; Doesn't work with assembly variant. :(
		; TODO: figure out before commiting. If takes more than 2 hours, copy assembly of create_sha512_hash2.
		;call create_sha512_hash
		; call create_sha512_hash2
		mov rcx, [rel buffer_ptr]
		mov rdx, BUFFER_SIZE - 1
		sub rsp, 32
		call SHA512Hash
		add rsp, 32
		
		push rax
		
		mov rcx, rax
		lea rdx, [rel expected_hash]
		mov r8, EXPECTED_HASH_LEN
		call cmp_counted_strs
		
		test rax, rax
		jnz .wrong_hash
		
		pop rcx
		call delete_sha512_hash
		
		lea rcx, [rel access_granted]
		sub rsp, 32
		call myprintf
		add rsp, 32
		
		; add rsp, BUFFER_SIZE
		mov rsp, rbp

		xor rax, rax
		ret

.wrong_hash:
		lea rcx, [rel access_denied]
		sub rsp, 32
		call myprintf
		add rsp, 32
		
		mov rcx, 2
		sub rsp, 32
		call ExitProcess

section .bss
stdin_handle: resq 1

BUFFER_SIZE equ 64   ; Make sure it is devisible by 8.
BUFFER_OVERRUN equ 8 ; Some overflow potential.
buffer_ptr: resq 1

EXPECTED_HASH_LEN equ 64
buffer_hash: resb EXPECTED_HASH_LEN

section .rdata
; Не %p, т.к. он не поддерживается моим printf.
greeting_fmt: db "Привет, 0x%016llx. Ты меня вызывал?", 0xA, 0
password_prompt: db "Да. Мой пароль: ", 0
access_granted: db "Доступ получен!", 0
access_denied: db "В доступе отказано!", 0
%macro hexstr_to_bytes 1
	%assign i 1
	%strlen len %1
	%assign niter (len+1)/2
	%rep niter
		%substr byte_str %1 i,2
		%deftok byte byte_str
		db 0x %+ byte
		%assign i i+2
	%endrep
%endmacro
; sha512sum password.txt
expected_hash: hexstr_to_bytes "49DCDA1CA8323CA974D0E2F916DB06BE29B9C646AE8B264BF13A4C5ACB4DC68B2286DD827E36F10803955F641175C62FF680CAE28A82133B3A15E2819210F4E5"
