MBALIGN  equ  1 << 0
MEMINFO  equ  1 << 1
MBFLAGS  equ MBALIGN | MEMINFO
MAGIC    equ  0x1BADB002
CHECKSUM equ -(MAGIC + MBFLAGS)

section .multiboot
align 4
	dd MAGIC
	dd MBFLAGS
	dd CHECKSUM
section .bss
align 16
stack_bottom:
resb 16384 * 4 ; 16 KiB * 4
stack_top:
section .boot
global _start:function (_start.end - _start)
_start:
	mov ecx, (initialPageDir - 0xC0000000)
	mov cr3, ecx

	mov ecx, cr4
	or ecx, 0x10
	mov cr4, ecx

	mov ecx, cr0
	or ecx, 0x80000000
	mov cr0, ecx

	jmp higherHalfMem
_start.end:

section .text:
higherHalfMem:
	mov esp, stack_top
	push eax
	push ebx
	xor ebp, ebp
	extern kmain
	call kmain

	cli
.hang:	hlt
	jmp .hang
.end:

section .data
align 4096
global initialPageDir
initialPageDir:
	dd 10000011b
	times 768-1 dd 0

	dd (0 << 22) | 10000011b
	dd (1 << 22) | 10000011b
	dd (2 << 22) | 10000011b
	dd (3 << 22) | 10000011b

	times 256 - 4 dd 0