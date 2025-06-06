MBALIGN  equ  1 << 0
MEMINFO  equ  1 << 1
VBE_MODE equ 1 << 2
MBFLAGS  equ MBALIGN | MEMINFO ; | VBE_MODE
MAGIC    equ  0x1BADB002
CHECKSUM equ -(MAGIC + MBFLAGS)

section .multiboot
align 4
	dd MAGIC
	dd MBFLAGS
	dd CHECKSUM

	dd 50
	dd 50
	dd 32

section .text
global _start:function (_start.end - _start)
_start:
	mov ebp, 0x8000
	mov esp, ebp
	push eax
	push ebx

	extern kmain
	call kmain
_start.end:
