#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "memory.h"
#include "fs.h"
#include "serial.h"
#include "pit.h"

#define DEFAULT_PROCESS_MEMSIZE 0x10000

typedef struct {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t esp;
    uint32_t ebp;
    uint32_t flags;
} generalRegs;

typedef struct {
    char name[32];
    bool waiting;
    uint32_t memStart;
    int memSize;
    uint32_t pcLoc;
    generalRegs regs;
    uint32_t pid;
} process;

extern process processes[256];

uint32_t spawnProcess(const char *path);
int getNextProcess();
void setProcessPC(int idx, uint32_t pc);
int getCurrentProcessIdx();
uint32_t getPCLoc(int idx);
void kill(int idx);
int getProcessIndexFromPID(uint32_t pid);
extern int schedulerProcessAt;
extern int processCount;
int getNextProcessDry();