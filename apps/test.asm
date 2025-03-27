[bits 32]

mov al, 'X'
int 0x40 ; Pix OS Write char syscall
halt:
    jmp halt