[bits 32]
global print
global exit
global getInput
global spawnProcess
global processRunning
global clearScr
global getProcCount
global getProcInfo
global getProcPIDFromIdx
global serialPrint
global shutdown
global reboot
global popMsg
global sendMsg
global changeLayout
global enable13hMode
global setPrimaryCoordinates
global setSecondaryCoordinates
global setColor
global putPixel
global drawLine
global loadFile
global getFileInfo

section .ackylib
print:
    ; Do not push EAX: The return value is stored in it
    push ebx
    push ecx

    mov ecx, [esp + 12]  ; Get the string address in EBX

    .loop:
        mov bl, [ecx]
        xor eax, eax     ; Print character syscall
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
    mov ecx, [esp + 12]
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

getProcCount:
    mov eax, 6
    int 0x40

    ret

clearScr:
    push ebx

    mov eax, 5
    int 0x40

    pop ebx
    ret

getProcInfo:
    push ebx
    push ecx

    mov eax, 7
    mov ebx, [esp + 12]
    mov ecx, [esp + 16]
    int 0x40

    pop ecx
    pop ebx
    ret

getProcPIDFromIdx:
    push ebx

    mov eax, 8
    mov ebx, [esp + 8]
    int 0x40

    pop ebx
    ret

serialPrint:
    push ebx
    push ecx

    mov ecx, [esp + 12]
    .loop:
        mov bl, [ecx]
        mov eax, 9
        int 0x40
        inc ecx
        cmp byte [ecx], 0
        jne .loop

    pop ecx
    pop ebx
    ret

shutdown:
    mov eax, 10
    mov ebx, 0
    int 0x40
    jmp $

reboot:
    mov eax, 10
    mov ebx, 1
    int 0x40
    jmp $

sendMsg:
    push ebx
    push ecx

    mov eax, 11
    mov ebx, 0
    mov edx, [esp + 12]
    mov ecx, [esp + 16]
    int 0x40

    pop ecx
    pop ebx
    ret

popMsg:
    push ebx
    push ecx

    mov eax, 11
    mov ebx, 1
    mov ecx, [esp + 12]
    int 0x40

    pop ecx
    pop ebx
    ret

changeLayout:
    push ebx

    mov ebx, [esp + 8]
    mov eax, 12
    int 0x40

    pop ebx
    ret

enable13hMode:
    push ebx

    mov eax, 13
    mov ebx, 0
    int 0x40

    pop ebx
    ret

setPrimaryCoordinates:
    push ebx
    push ecx
    push edx

    mov eax, 13
    mov ebx, 1
    mov ecx, [esp + 16]
    mov edx, [esp + 20]
    int 0x40

    pop edx
    pop ecx
    pop ebx
    ret

setSecondaryCoordinates:
    push ebx
    push ecx
    push edx

    mov eax, 13
    mov ebx, 2
    mov ecx, [esp + 16]
    mov edx, [esp + 20]
    int 0x40

    pop edx
    pop ecx
    pop ebx
    ret

setColor:
    push ebx
    push ecx

    mov eax, 13
    mov ebx, 3
    mov ecx, [esp + 12]
    int 0x40

    pop ecx
    pop ebx
    ret

putPixel:
    push ebx
    push ecx
    push edx

    mov eax, 13
    mov ebx, 4
    mov ecx, [esp + 16]
    mov edx, [esp + 20]
    int 0x40

    pop edx
    pop ecx
    pop ebx
    
drawLine:
    push ebx

    mov eax, 13
    mov ebx, 5
    int 0x40

    pop ebx
    ret

loadFile:
    push ebx
    push ecx

    mov eax, 14
    mov ebx, [esp + 12]  ; Path
    mov ecx, [esp + 16]  ; Buffer
    int 0x40

    pop ecx
    pop ebx
    ret

getFileInfo:
    push ebx
    push ecx

    mov eax, 15
    mov ebx, [esp + 12] ; Path
    mov ecx, [esp + 16] ; Struct
    int 0x40

    pop ecx
    pop ebx
    ret