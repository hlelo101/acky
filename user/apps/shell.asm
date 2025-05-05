[bits 32]
loop:
    mov byte [uinput], 0

    mov ecx, ps1
    call print_string

    mov eax, 1
    mov ebx, uinput
    int 0x40

    cmp byte [uinput], 0
    je loop

    mov eax, 2
    mov ebx, uinput
    mov ecx, processName
    int 0x40

    cmp eax, 0
    jne .err

    jmp loop
.err:
    mov ecx, notFound
    call print_string
    jmp loop

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

ps1 db 'Acky> ', 0
processName db 'Shell-started', 0
uinput times 210 db 0
notFound db 'Program not found', 10, 0
emptyMsg db 'Empty input', 10, 0
numOutput dw 0
ctestPath db 'A:/PROGDAT/CTEST.BIN', 0