global trampoline
global proc_ip
global proc_flags
global proc_cs
global proc_regs_eax
global proc_regs_ebx
global proc_regs_ecx
global proc_regs_edx
global proc_regs_esp
global proc_regs_ebp
global proc_regs_esi
global proc_regs_edi

trampoline:
    mov eax, [proc_regs_eax]
    mov ebx, [proc_regs_ebx]
    mov ecx, [proc_regs_ecx]
    mov edx, [proc_regs_edx]
    mov esi, [proc_regs_esi]
    mov edi, [proc_regs_edi]
    mov ebp, [proc_regs_ebp]

    ; Set the data segments
    push dx
    mov dx, 0x0F ; 0x0C for ring 3
    mov ds, dx
    mov es, dx
    mov fs, dx
    mov gs, dx
    pop dx

    push dword 0x0F                 ; Set SS
    push dword cs:[proc_regs_esp]   ; Set ESP
    push dword cs:[proc_flags]      ; Restore the flags
    push dword 0x07                 ; Set CS, 0x04 for ring 0
    push dword cs:[proc_ip]         ; Set EIP
    ; All of these values will be popped off by iret
    iret

proc_ip dd 0
proc_flags dd 0
proc_regs_eax dd 0
proc_regs_ebx dd 0
proc_regs_ecx dd 0
proc_regs_edx dd 0
proc_regs_esp dd 0
proc_regs_ebp dd 0
proc_regs_esi dd 0
proc_regs_edi dd 0
