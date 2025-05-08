[bits 32]
db "Test 2", 0, "                        ", 0
dd thing
db "AEF"

thing:
    mov ecx, 10000000
nopLoop:
    nop
    loop nopLoop

    mov eax, 0
    mov ebx, '2'
    int 0x40

    mov eax, 1
    mov ebx, thing
    int 0x40

    mov ecx, thing
    call print_string
    
    mov eax, 3
    int 0x40

    ; String address in ECX
print_string:
	pusha
	mov eax, 0 ; System call number
.loop:
	mov bl, [ecx]
	int 0x40
	inc ecx
	cmp byte [ecx], 0
	jne .loop

	popa
	ret

