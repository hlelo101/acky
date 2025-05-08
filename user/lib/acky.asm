[bits 32]
global print
global exit
global getInput
global spawnProcess
global processRunning

section .ackylib
print:
    ; Do not push EAX: The return value is stored in it
    push ebx
    push ecx

    mov ecx, [esp + 12]  ; Get the string address in EBX
    xor eax, eax         ; Print character syscall

    .loop:
        mov bl, [ecx]
        int 0x40

        inc ecx
        cmp byte [ecx], 0
        jne .loop

    pop ecx
    pop ebx
    ret

getInput:
    push ebx

    mov eax, 1
    mov ebx, [esp + 8]
    int 0x40

    pop ebx
    ret

spawnProcess:
    push ebx

    mov eax, 2
    mov ebx, [esp + 8]
    int 0x40

    pop ebx
    ret

processRunning:
    push ebx

    mov eax, 4
    mov ebx, [esp + 8]
    int 0x40

    pop ebx
    ret

exit:
    mov eax, 3
    int 0x40
    jmp $ ; How did you get here