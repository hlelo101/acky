#include "memory.h"
#include "herr.h"
#include <stddef.h>
#include <stdbool.h>

static uint32_t pageFrameMin;
static uint32_t pageFrameMax;
static uint32_t totalAlloc;
uint8_t phyicalMemBitmap[NUM_PAGE_FRAMES / 8]; // TODO: Do that dynamically pwease
static uint32_t pageDirs[NUM_PAGE_DIRS][1024]__attribute__((aligned(4096)));
static uint8_t pageDirUsed[NUM_PAGE_DIRS];

static uint32_t heapStart;
static uint32_t heapSize;
static uint32_t threshold;
static bool kmallocInitialized = false;
int memNumVpages;

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
 
uint32_t pmmAllocPageFrame() {
    uint32_t start = pageFrameMin / 8 + ((pageFrameMin & 7) != 0 ? 1 : 0);
    uint32_t end = pageFrameMax / 8 - ((pageFrameMax & 7) != 0 ? 1 : 0);

    for(uint32_t b = start; b < end; b++) {
        uint8_t byte = phyicalMemBitmap[b];
        if(byte == 0xFF) continue;

        for(uint32_t i = 0; i < 8; i++) {
            bool used = byte >> i & 1;
            if(!used) {
                // byte ^= (-1 ^byte) & (1 << i);
                byte |= (1 << i);
                phyicalMemBitmap[b] = byte;
                totalAlloc++;

                uint32_t addr = (b*8+i) * 0x1000;
                return addr;
            }
        }
    }

    return 0;
}

uint32_t* memGetCurrentPageDir() {
    uint32_t pd;
    asm volatile ("mov %%cr3, %0" : "=r"(pd));
    pd += KERNEL_START;

    return (uint32_t*)pd;
}

void memChangePageDir(uint32_t* pd) {
    pd = (uint32_t*)(((uint32_t)pd) - KERNEL_START);
    asm volatile ("mov %0, %%eax\nmov %%eax, %%cr3" :: "m"(pd));
}

void syncPageDirs() {
    for(int i = 0; i < NUM_PAGE_DIRS; i++) {
        if(pageDirUsed[i]) {
            uint32_t* pageDir = pageDirs[i];

            for(int i = 768; i < 1023; i++) {
                pageDir[i] = initialPageDir[i] & ~POWNER;
            }
        }
    }
}

void memMapPage(uint32_t virtAddr, uint32_t physAddr, uint32_t flags) {
    memNumVpages = 0;
    uint32_t *prevPageDir = 0;
    if(virtAddr < KERNEL_START) herr("Trying to map a page below the kernel start");
    if(virtAddr >= KERNEL_START) {
        prevPageDir = memGetCurrentPageDir();
        if(prevPageDir != initialPageDir) memChangePageDir(initialPageDir);
    }

    uint32_t pdIndex = virtAddr >> 22;
    // uint32_t ptIndex = (virtAddr >> 12) & 0x3FF;

    uint32_t* pageDir = REC_PAGEDIR;
    uint32_t* pt = REC_PAGETABLE(pdIndex);
    
    if(!pageDir[pdIndex] & PPRESENT) {
        uint32_t ptPaddr = pmmAllocPageFrame();
        pageDir[pdIndex] = ptPaddr | PPRESENT | PWRITE | POWNER | flags;
        invalidate(virtAddr);

        for(uint32_t i = 0; i<1024; i++) {
            pt[i] = 0;
        }
    }

    pt[pdIndex] = physAddr | PPRESENT | flags;
    memNumVpages++;
    invalidate(virtAddr);

    if(prevPageDir != 0) {
        syncPageDirs();
        if(prevPageDir != initialPageDir) memChangePageDir(prevPageDir);
    }
}

void changeHeapSize(int newSize) {
    int oldPageTop = CEIL_DIV(heapSize, 0x1000);
    int newPageTop = CEIL_DIV(newSize, 0x1000);

    int diff = newPageTop - oldPageTop;

    for(int i = 0; i<diff; i++) {
        uint32_t phys = pmmAllocPageFrame();
        memMapPage(KERNEL_MALLOC + oldPageTop * 0x1000 + i * 0x1000, phys, PWRITE);
    }
}

void initKmalloc(uint32_t initialHeapSize) {
    heapStart = KERNEL_MALLOC;
    heapSize = 0;
    threshold = 0;
    kmallocInitialized = true;

    changeHeapSize(initialHeapSize);
}