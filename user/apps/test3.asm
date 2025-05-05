; [bits 32]

; mov eax, 0xABCD
; jmp $

; halt:
;     mov al, 'C'
;     int 0x40 ; syscally syscall :3
;     jmp halt

[bits 32]
halt:
    mov ecx, string
    call print_string
    jmp halt

; String address in ECX
print_string:
	pusha
	mov eax, 0 ; System call number
print_string_loop:
	mov bl, [ecx]
	int 0x40
	inc ecx
	cmp byte [ecx], 0
	jne print_string_loop
	
	popa
	ret

string db 'hai there this is a rather long message to test something', 10, 0