[bits 32]
mov ecx, string
call print_string
halt:
    jmp halt

; String address in EBX
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

string db 'Hello from the init system!', 10, 0