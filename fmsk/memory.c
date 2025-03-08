#include "memory.h"
#include <stddef.h>

static uint32_t pageFrameMin;
static uint32_t pageFrameMax;
static uint32_t totalAlloc;
uint8_t phyicalMemBitmap[NUM_PAGE_FRAMES / 8]; // TODO: Do that dynamically pwease
static uint32_t pageDirs[NUM_PAGE_DIRS][1024]__attribute__((aligned(4096)));
static uint8_t pageDirUsed[NUM_PAGE_DIRS];

void *memcpy(void *dest, const void *src, int n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    
    while (n--) {
        *d++ = *s++;
    }
    
    return dest;
}

void *memset(void *ptr, int value, size_t num) {
    unsigned char *p = ptr;
    while (num--) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}

void invalidate(uint32_t virtAddr) {
    __asm__ __volatile__ ("invlpg %0" :: "m"(virtAddr));
}

void pmmInit(uint32_t physSize, uint32_t physStart) {
    pageFrameMin = CEIL_DIV(physStart, 0x1000);
    pageFrameMax = physSize / 0x1000;
    totalAlloc = 0;

    memset(phyicalMemBitmap, 0, sizeof(phyicalMemBitmap));
}

void initMemory(uint32_t physSize, uint32_t physStart) {
    // We don't need the first entry anymore
    initialPageDir[0] = 0;
    invalidate(0);
    initialPageDir[1023] = ((uint32_t) initialPageDir - KERNEL_START) | PPRESENT | PWRITE;
    invalidate(0xFFFFF000);

    pmmInit(physSize, physStart);
    memset(pageDirs, 0, 0x1000 * NUM_PAGE_DIRS);
    memset(pageDirUsed, 0, NUM_PAGE_DIRS);
}
