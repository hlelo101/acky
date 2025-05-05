/* This is the linker script that defines how C code should be compiled for Acky. */
ENTRY(_start)

SECTIONS {
    . = 0x0000;

    .text : {
        KEEP(*(.text.header))
        KEEP(*(.text._start))
        *(.text)
        *(.text*)
    }
    .rodata : {
        *(.rodata)
        *(.rodata*)
    }
    .data : {
        *(.data)
        *(.data*)
    }
    .bss : {
        *(.bss)
        *(COMMON)
    }
}