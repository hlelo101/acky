#include "gdt.h"

GDTR gdtr;
gdtDescriptor gdt[3];

void setGDTEntry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt[index].baseLow = (base & 0xFFFF);
    gdt[index].baseMiddle = (base >> 16) & 0xFF;
    gdt[index].baseHigh = (base >> 24) & 0xFF;
    gdt[index].limitLow = (limit & 0xFFFF);
    gdt[index].granularity = ((limit >> 16) & 0x0F) | (granularity & 0xF0);
    gdt[index].access = access;
}


void initGDT() {
    // Set up the GDT
    setGDTEntry(0, 0, 0, 0, 0); // Null segment
    setGDTEntry(1, 0, 0x03FF, 0x9A, 0xCF); // Code segment
    setGDTEntry(2, 0, 0xFFFF, 0x92, 0xCF); // Data segment

    // The GDTR
    gdtr.size = sizeof(gdt) - 1;
    gdtr.offset = (uint32_t)&gdt;

    // Then load it!
    asm volatile("lgdt (%0)" : : "r"(&gdtr));

    // Longjump, finish setting the GDT up
    asm volatile("ljmp $0x08, $loadGDTjmp");
    asm volatile(
        "loadGDTjmp:\n"
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        :
        :
        : "%ax"
    );
}

// Used to be this: (it was just rewritten in C because it's easier to then make modifications to it)
// [BITS 32]

// gdt:
//     ; Null descriptor
//     dq 0
//     ; Kernel Mode Code Segment Descriptor
//     dw 0x03FF
//     dw 0
//     db 0
//     db 10011010b
//     db 11001111b
//     db 0
//     ; Kernel Mode Data Segment Descriptor
//     dw 0xffff
//     dw 0
//     db 0
//     db 10010010b
//     db 11001111b
//     db 0
// gdt_end:

// gdtr:
//     dw gdt_end - gdt - 1
//     dd gdt

// global initGDT
// initGDT:
//     cli
// loadGDT:
//     lgdt [gdtr]

//     ; Grub should have already put the CPU in protected mode but eh
//     mov eax, cr0
//     or eax, 1
//     mov cr0, eax

//     jmp 0x08:loadGDTjmp
// loadGDTjmp:
//     mov ax, 0x10
//     mov ds, ax
//     mov es, ax
//     mov fs, ax
//     mov gs, ax
//     mov ss, ax

//     ret