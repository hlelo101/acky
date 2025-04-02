[bits 32]

halt:
    mov al, 'C'
    int 0x40 ; syscally syscall :3
    hlt
    jmp halt