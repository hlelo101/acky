global switchRealMode

switchRealMode:
    cli

    mov eax, cr0
    and eax, 0xFFFFFFFE
    mov cr0, eax

    xchg bx, bx
    jmp 0x0000:realMode
[bits 16]
realMode:
    jmp 0x0000:realFlushCS
realFlushCS:
    mov ax, 0
    mov gs, ax
    mov fs, ax
    mov ss, ax
    mov es, ax
    mov ds, ax

    ; We should be in real mode :3
    ; For now, just halt
    jmp $
