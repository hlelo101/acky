#include "process.h"
#include "serial.h"

int processCount = 0;
int schedulerProcessAt = 0;
process processes[256];

int createProcessEntry(const char *name) {
    strcpy(processes[processCount].name, name);
    processes[processCount].used = true;
    processes[processCount].memStart = allocMem(DEFAULT_PROCESS_MEMSIZE);
    processes[processCount].memSize = DEFAULT_PROCESS_MEMSIZE;
    processCount++;
    
    return processCount - 1;
}

int getNextProcess() {
    if(processCount == 0) return -1;
    schedulerProcessAt++;
    if(schedulerProcessAt >= processCount) schedulerProcessAt = 0;
    return schedulerProcessAt;
}

int getCurrentProcessIdx() {
    if(processCount == 0) return -1;
    return schedulerProcessAt;
}

uint32_t getPCLoc(int idx) {
    if(idx < 0 || idx >= processCount) return 0;
    return processes[idx].pcLoc;
}

void setProcessPC(int idx, uint32_t pc) {
    if(idx < 0 || idx >= processCount) return;
    processes[idx].pcLoc = pc;
}

void setProcessRegs(int idx, generalRegs *regs) {
    if(idx < 0 || idx >= processCount) return;
    processes[idx].regs = *regs;
}

void getProcessRegs(int idx, generalRegs *regs) {
    if(idx < 0 || idx >= processCount) return;
    *regs = processes[idx].regs;
}

void spawnProcess(const char *name, const char *path) {
    uint8_t processBuffer[2048] = {0};
    fsReadFile(path, processBuffer);
    if(processBuffer[0] == 0) return; // File not found
    int processIndex = createProcessEntry(name);
    memcpy((void *)processes[processIndex].memStart, processBuffer, 2048);
    processes[processIndex].pcLoc = processes[processIndex].memStart;
}

// Handle LDT stuff