#pragma once
#include <stdint.h>

#define NUM_LDT_ENTRIES 3
#define NUM_GDT_ENTRIES 7

typedef struct {
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMiddle;
    uint8_t access;
    uint8_t granularity; // Flags + Limit high
    uint8_t baseHigh;
}__attribute((packed)) gdtDescriptor;

typedef struct {
    uint16_t size;
    uint32_t offset;
}__attribute__((packed)) GDTR;

typedef struct {
	uint32_t prev_tss;
    // Values needed to switch back to kernel mode
	uint32_t esp0;
	uint32_t ss0;
	// Unused :(
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
} __attribute__((packed)) tss;


extern gdtDescriptor gdt[NUM_GDT_ENTRIES];
extern gdtDescriptor currentLDT[NUM_LDT_ENTRIES];

void initGDT();
void setGDTEntry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);
void setLDTEntry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);
extern tss realTSS;