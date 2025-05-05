[bits 32]

start:

thing:
    mov eax, 0
    mov ebx, '2'
    int 0x40

    mov ecx, 10000000
nopLoop:
    nop
    loop nopLoop
    jmp thing