#include "memory.h"
#include "herr.h"
#include "kmain.h"
#include <stddef.h>
#include <stdbool.h>

memEntry memList[MEM_LIST_SIZE];
int memIndex = 1;

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

void strcpy(char *dest, const char *src) {
    while(*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = 0;
}

int strcmp(const char *s1, const char *s2) {
    while(*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

uint32_t allocMem(uint32_t size) {
    if(memList[memIndex - 1].startAddr + size > getSystemMemory()) herr("Out of memory");
    if(memIndex >= MEM_LIST_SIZE) herr("Memory list overflow");
    memList[memIndex].startAddr = memList[memIndex - 1].startAddr + memList[memIndex - 1].size;
    memList[memIndex].size = size;
    memIndex++;

    return memList[memIndex - 1].startAddr;
}

void initMem() {
    // Initialize the first "null" entry
    memList[0].startAddr = 0;
    memList[0].size = 0;
    // Allocate memory for the kernel
    allocMem(0x300000); // 3MB
}

int freeMem(uint32_t memStart) {
    if(memStart <= 0) return -1;
    for(int i = 0; i < memIndex; i++) {
        if(memList[i].startAddr == memStart) {
            uint32_t size = memList[i].size;
            for(int j = i; j < memIndex - 1; j++) {
                memList[j + 1].startAddr -= size;
                memList[j] = memList[j + 1];
            }
            memIndex--;
            
            memcpy((void *)memStart, (void *)(memStart + size), (memList[memIndex].startAddr + memList[memIndex].size) - memStart);
            serialSendString("[freeMem()]: Deallocated "); serialSendInt(size); serialSendString(" bytes\n");
            return memStart;
        }
    }
    
    return -1; // Not found
}