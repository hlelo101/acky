[bits 32]

movthing:
; xchg bx, bx
mov eax, 0xABCD
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
jmp movthing

; notequalwtf:
;     mov al, 'X'
;     int 0x40 ; syscally syscall :3
;     hlt
;     jmp halt

; halt:
;     mov ebx, string
; 	; xchg bx, bx
;     call print_string
;     jmp halt

; ; String address in EBX
; print_string:
; 	pusha
; print_string_loop:
; 	mov al, [ebx]
; 	int 0x40
; 	inc ebx
; 	cmp byte [ebx], 0
; 	jne print_string_loop
	
; 	popa
; 	ret

; string db 'Hello from test2.asm', 10, 0