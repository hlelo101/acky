#pragma once
#include <stdint.h>

typedef struct {
    uint16_t size;
    uint32_t addr;
}__attribute((packed)) idtrDesc;

typedef struct {
    uint16_t offsetLow;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t offsetHigh;
}__attribute__((packed)) gate;

void initIDT();
void PICMap(int masterOffset, int slaveOffset, uint8_t masterMasks, uint8_t slaveMasks);
void setIDTGate(int index, uint32_t address, uint16_t selector, uint8_t flags);