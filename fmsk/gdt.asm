[BITS 32]

gdt:
    ; Null descriptor
    dq 0
    ; Kernel Mode Code Segment Descriptor
    dw 0x03FF
    dw 0
    db 0
    db 10011010b
    db 11001111b
    db 0
    ; Kernel Mode Data Segment Descriptor
    dw 0xffff
    dw 0
    db 0
    db 10010010b
    db 11001111b
    db 0
gdt_end:

gdtr:
    dw gdt_end - gdt - 1
    dd gdt


global initGDT
initGDT:
    cli
loadGDT:
    lgdt [gdtr]

    ; Grub should have already put the CPU in protected mode but eh
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:loadGDTjmp
loadGDTjmp:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ret