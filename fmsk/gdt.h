#pragma once
#include <stdint.h>

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

void initGDT();