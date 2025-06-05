#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    char signature[4]; // KFE1
    uint16_t loadAddr;
    uint8_t numFuncs;
    uint16_t funcsAddr[];
}__attribute__((packed)) kfeHeader;

typedef uint32_t (*func_t)(void);

extern uint16_t VBELoadAddr;
unsigned int getSystemMemory();
extern unsigned int systemMemoryB;
extern bool canPreempt;
extern bool systemInitialized;
uint32_t loadKFE(const char *path);
uint32_t callKFEFunction(uint8_t index, uint32_t loadAddr);
