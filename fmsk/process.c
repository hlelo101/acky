#include "process.h"

int processCount = 0;
process processes[256];

int createProcessEntry(const char *name) {
    strcpy(processes[processCount].name, name);
    processes[processCount].used = true;
    processes[processCount].memStart = allocMem(0x1000);
    processes[processCount].memSize = 0x1000;
    processCount++;
    
    return processCount - 1;
}

void spawnProcess(const char *name, const char *path) {
    uint8_t processBuffer[2048] = {0};
    fsReadFile(path, processBuffer);
    if(processBuffer[0] == 0) return; // File not found
    int processIndex = createProcessEntry(name);
    memcpy((void *)processes[processIndex].memStart, processBuffer, 2048);
    asm volatile ("jmp %0" : : "r"(processes[processIndex].memStart));
}