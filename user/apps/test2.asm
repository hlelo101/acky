[bits 32]
db "Test 2", 0, "                        ", 0
dd thing
db "AEF"

thing:
    mov eax, 0
    mov ebx, '2'
    int 0x40

    mov ecx, 10000000
nopLoop:
    nop
    loop nopLoop
    jmp thing