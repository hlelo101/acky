#include <stdint.h>
#include <stdbool.h>
#include "memory.h"
#include "fs.h"

#define DEFAULT_PROCESS_MEMSIZE 0x10000

// typedef struct {

// }__attribute__((packed)) LDTEntry;

typedef struct {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
} generalRegs;

typedef struct {
    char name[32];
    bool used;
    uint32_t memStart;
    int memSize;
    uint32_t pcLoc;
    generalRegs regs;
} process;

extern process processes[256];

void spawnProcess(const char *name, const char *path);
int getNextProcess();
void setProcessPC(int idx, uint32_t pc);
int getCurrentProcessIdx();
uint32_t getPCLoc(int idx);