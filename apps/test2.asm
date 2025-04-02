[bits 32]

section .text
halt:
    mov si, string
    ; mov si, 'H'
    call print_str
    ; hlt
    jmp halt

print_str:
    ; String in SI
    cmp byte [si], 0
    je print_done
    mov al, [si]
    int 0x40 ; Pix OS Write char syscall
    hlt
    inc si
    jmp print_str
print_done:
    ret

section .data
string db 'Hello from test2.asm', 0