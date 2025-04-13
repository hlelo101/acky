#include "gdt.h"

GDTR gdtr;
gdtDescriptor gdt[NUM_GDT_ENTRIES];
gdtDescriptor currentLDT[NUM_LDT_ENTRIES];

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

void updateLDTBase(uint32_t newBase) {
    for(int i = 0; i < NUM_LDT_ENTRIES; i++) {
        currentLDT[i].baseLow = (newBase & 0xFFFF);
        currentLDT[i].baseMiddle = (newBase >> 16) & 0xFF;
        currentLDT[i].baseHigh = (newBase >> 24) & 0xFF;
    }
}



void initGDT() {
    // Set up the initial LDT
    setLDTEntry(0, 0, 0x100, 0x9A, 0xCF); // Code segment
    setLDTEntry(1, 0, 0x100, 0x92, 0xCF); // Data segment

    // Set up the GDT
    setGDTEntry(0, 0, 0, 0, 0); // Null segment
    setGDTEntry(1, 0, 0x03FF, 0x9A, 0xCF); // Code segment
    setGDTEntry(2, 0, 0xFFFF, 0x92, 0xCF); // Data segment
    setGDTEntry(3, (uint32_t)&currentLDT, sizeof(currentLDT) - 1, 0x82, 0xCF); // LDT segment

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

    asm volatile("lldt %0" : : "r"(3 * 8));
}