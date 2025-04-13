#pragma once
#include <stdint.h>

#define NUM_LDT_ENTRIES 2
#define NUM_GDT_ENTRIES 4

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

extern gdtDescriptor gdt[4];
extern gdtDescriptor currentLDT[2];
void initGDT();
void setGDTEntry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);
void updateLDTBase(uint32_t newBase);