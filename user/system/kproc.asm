; Acky executable file header
db "Kernel", 0, "                        ", 0	; Name
dd start								; Entry point
db "AEF"								; Signature

start:
jmp $