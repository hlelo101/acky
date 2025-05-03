; [bits 32]

; mov eax, 0xABCD
; jmp $

; halt:
;     mov al, 'C'
;     int 0x40 ; syscally syscall :3
;     jmp halt

[bits 32]
halt:
    mov ebx, string
    call print_string
    jmp halt

; String address in EBX
print_string:
	pusha
print_string_loop:
	mov al, [ebx]
	int 0x40
	inc ebx
	cmp byte [ebx], 0
	jne print_string_loop
	
	popa
	ret

string db 'hai there this is a rather long message to test something', 10, 0