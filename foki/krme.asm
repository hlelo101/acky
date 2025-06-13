; Kernel real mode extension. Provides access to things like VBE by enabling real mode, 
; calling some BIOS/VGA BIOS functions and re-enabling protected mode. While Foki tries
; to be as independant as possible from the BIOS, it cannot do things like use VBE 
; without it (VBE is part of the graphic card's own BIOS and can only be accessed in 
; real mode).
[org 0x1000]
db "KFE1"   ; Kernel function extension signature, version 1
dw 0x1000   ; Load address
db 2        ; Number of functions
dw chVBEMode
dw setTextMode

[bits 32]
chVBEMode:
    pusha
    mov [savedEBP], ebp 
    mov [savedESP], esp
    mov bp, 0xFFF
    mov sp, bp
    call setRealMode
    [bits 16]
    ; NOW we can set the VBE mode; mode is 0x4115
    ; Set the mode
    mov ax, 0x4F02
    mov bx, 0x4118
    int 0x10
    ; Get the buffer address
    mov ax, 0x4F01
    mov cx, 0x4118
    mov di, vbeRetInfo
    int 0x10
    call setProtectedMode
    [bits 32]
    mov ebp, [savedEBP]
    mov esp, [savedESP]
    popa
    mov eax, vbeRetInfo
    ret

setTextMode:
    pusha
    mov [savedEBP], ebp 
    mov [savedESP], esp
    mov bp, 0xFFF
    mov sp, bp
    call setRealMode
    [bits 16]
    ; Set the mode
    mov ah, 0x00
    mov al, 0x03
    int 0x10
    call setProtectedMode
    [bits 32]
    mov ebp, [savedEBP]
    mov esp, [savedESP]

    popa
    mov eax, 0
    ret

; Switch between modes
ivt:
    dw 0x3FF
    dd 0

gdt_start:
gdt_null:
    dq 0
gdt_code16:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 00000000b
    db 0x00

gdt_data16:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 00000000b
    db 0x00

gdt_end:
gdtr:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[bits 32]
setRealMode:
    cli
    ; Setup some 16-bit entries
    lgdt [gdtr]
    jmp 0x08:.gdtLoad

[bits 16]
.gdtLoad:
    mov eax, 0x10
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax

    mov eax, cr0
    and eax, 0x7FFFFFFE
    mov cr0, eax
    jmp 0:realModeEntry
realModeEntry:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    lidt [ivt]
    ; Now we're in real mode!
    ret

tempGDT:
    ; Null
    dq 0
    ; Code segment
    dq 0x00CF9A000000FFFF
    ; Data segment
    dq 0x00CF92000000FFFF
tempGDTR:
    dw tempGDTREnd - tempGDT - 1
    dd tempGDT
tempGDTREnd:
[bits 16]
setProtectedMode:
    cli

    lgdt [tempGDTR]
    mov eax, cr0
    or al, 1 
    mov cr0, eax
    jmp 0x08:.gdtLoad
[bits 32]
.gdtLoad:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    ret

savedESP dd 0 
savedEBP dd 0

vbeRetInfo:
