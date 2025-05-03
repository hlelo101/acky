global trampoline
global proc_ip
global proc_flags
global proc_cs
global proc_regs_eax
global proc_regs_ebx
global proc_regs_ecx
global proc_regs_edx
global proc_regs_esp

trampoline:
    mov eax, [proc_regs_eax]
    mov ebx, [proc_regs_ebx]
    mov ecx, [proc_regs_ecx]
    mov edx, [proc_regs_edx]

    push eax

    ; Set DS
    mov ax, 0x0F ; 0x0C for ring 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    pop eax

    mov ebp, 4096
    ; NOTE: Stack segment selector is 0x17
    push 0x17                       ; Set SS
    push dword cs:[proc_regs_esp]   ; Set ESP
    push dword cs:[proc_flags]      ; Restore the flags
    push 0x07                       ; Set CS, 0x04 for ring 0
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