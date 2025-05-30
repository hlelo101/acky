#include "gdt.h"

GDTR gdtr;
gdtDescriptor gdt[NUM_GDT_ENTRIES];
gdtDescriptor currentLDT[NUM_LDT_ENTRIES];
tss realTSS;

void setGDTEntry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt[index].baseLow = (base & 0xFFFF);
    gdt[index].baseMiddle = (base >> 16) & 0xFF;
    gdt[index].baseHigh = (base >> 24) & 0xFF;
    gdt[index].limitLow = (limit & 0xFFFF);
    gdt[index].granularity = ((limit >> 16) & 0x0F) | (granularity & 0xF0);
    gdt[index].access = access;
}

void setLDTEntry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    currentLDT[index].baseLow = (base & 0xFFFF);
    currentLDT[index].baseMiddle = (base >> 16) & 0xFF;
    currentLDT[index].baseHigh = (base >> 24) & 0xFF;
    currentLDT[index].limitLow = (limit & 0xFFFF);
    currentLDT[index].granularity = ((limit >> 16) & 0x0F) | (granularity & 0xF0);
    currentLDT[index].access = access;
}

void initGDT() {
    // Set up the initial LDT
    setLDTEntry(0, 0, 0x1000, 0xFA, 0xCF);                                       // Code segment; 0x9A for ring 0
    setLDTEntry(1, 0, 0x1000, 0xF2, 0xCF);                                       // Data segment; 0x92 for ring 0
    // Set up the GDT
    setGDTEntry(0, 0, 0, 0, 0);                                                  // Null segment
    setGDTEntry(1, 0, 0xFFFFF, 0x9A, 0xCF);                                      // Code segment
    setGDTEntry(2, 0, 0xFFFFF, 0x92, 0xCF);                                      // Data segment
    setGDTEntry(3, (uint32_t)&currentLDT, sizeof(currentLDT) - 1, 0xE2, 0xCF);   // LDT segment; 0x82 for ring 0
    setGDTEntry(5, 0x300000, 0xFFFFF, 0xFA, 0xCF);                               // Ring 3 code segment
    setGDTEntry(6, 0x300000, 0xFFFFF, 0xF2, 0xCF);                               // Ring 3 data segment
    // TSS
    asm volatile("movl %%ebp, %0" : "=r"(realTSS.esp0));
    realTSS.ss0 = 0x10;
    setGDTEntry(4, (uint32_t)&realTSS, sizeof(realTSS) - 1, 0x89, 0x00);

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
        : : : "%ax"
    );

    asm volatile("lldt %0" : : "r"(0x18)); // LDT
    asm volatile("ltr %0" : : "r" (0x20)); // TSS
}