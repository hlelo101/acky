#pragma once
#include "utils.h"
#include <stdint.h>
#include <stddef.h>

#define KERNEL_START 0xC0000000
#define PPRESENT (1 << 0)
#define PWRITE (1 << 1)
#define POWNER (1 << 9)
#define NUM_PAGE_DIRS 256
#define NUM_PAGE_FRAMES 0x100000000 / 0x1000 / 8
#define KERNEL_MALLOC 0xD0000000
#define REC_PAGEDIR ((uint32_t*)0xFFFFF000)
#define REC_PAGETABLE(i) ((uint32_t*)(0xFFC00000 + ((i) << 12)))

extern uint32_t initialPageDir[1024];

void *memcpy(void *dest, const void *src, int n);
void *memset(void *ptr, int value, size_t num);
void initMemory(uint32_t memHigh, uint32_t memSize);
void initKmalloc(uint32_t initialHeapSize);
void changeHeapSize(int newSize);
uint32_t pmmAllocPageFrame();