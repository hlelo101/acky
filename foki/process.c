#include "process.h"

int processCount = 0;
int schedulerProcessAt = 0;
process processes[256];

int createProcessEntry(const char *name, uint32_t fileSize) {
    strcpy(processes[processCount].name, name);
    processes[processCount].pcLoc = 0;
    processes[processCount].regs.flags = 0x3206; // 0x206 for ring 0
    processes[processCount].regs.esp = 4096;
    processes[processCount].regs.ebp = 4096;
    processes[processCount].waiting = false;
    processes[processCount].memStart = allocMem(fileSize);
    processes[processCount].pid = tick + processCount;
    serialSendString("[createProcessEntry()]: New process \""); serialSendString(name);
    serialSendString("\" , memory allocated at: "); serialSendInt(processes[processCount].memStart); 
    serialSendString(" PID: "); serialSendInt(processes[processCount].pid); serialSend('\n');
    processes[processCount].memSize = fileSize;
    processCount++;
    
    return processCount - 1;
}

int getProcessIndexFromPID(uint32_t pid) {
    for(int i = 1; i<processCount; i++) {
        if(processes[i].pid == pid) return i;
    }
    return -1; // Not found
}

int getNextProcess() {
    if(processCount == 0) return -1;

    pickOther:
    schedulerProcessAt++;
    if(schedulerProcessAt >= processCount) schedulerProcessAt = 1;
    if(schedulerProcessAt == 0) schedulerProcessAt = 1; // Skip the kernel process
    if(processes[schedulerProcessAt].waiting) goto pickOther;

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

int spawnProcess(const char *name, const char *path) {
    fileInfo info;
    if(fsGetFileInfo(path, &info) == -1) return -1; // File not found
    uint8_t processBuffer[info.size < 2048 ? 2048 : info.size];

    fsReadFile(path, processBuffer, &info);
    int processIndex = createProcessEntry(name, info.size);
    memcpy((void *)processes[processIndex].memStart, processBuffer, 2048);

    return processes[processIndex].pid;
}

void kill(int pid) {
    const int idx = getProcessIndexFromPID(pid);
    if(idx == -1) return;
    
    serialSendString("[kill()]: Killing process \""); serialSendString(processes[idx].name); serialSendString("\"\n");
    const uint32_t memSize = processes[idx].memSize;
    freeMem(processes[idx].memStart);
    for(int i = idx; i < processCount - 1; i++) {
        processes[i + 1].memStart -= memSize;
        processes[i] = processes[i + 1];
    }
    processCount--;
}