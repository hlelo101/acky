[bits 32]
; Acky executable file header
db "Init", 0, "                          ", 0   ; Name
dd start                                        ; Entry point
db "AEF"                                        ; Signature

start:
mov ecx, string
call print_string

mov eax, 2
mov ebx, procPath
int 0x40

halt:
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

string db 10, 'Welcome to Acky OS!', 10, 0
procPath db 'A:/PROGDAT/SHELL.AEF', 0