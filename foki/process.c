#include "process.h"

int processCount = 0;
int schedulerProcessAt = 0;
process processes[256];

int createProcessEntry(const char *name, uint32_t fileSize) {
    uint32_t fileSizeWithStack = fileSize + (4096 * 4 + 1);

    strcpy(processes[processCount].name, name);
    processes[processCount].pcLoc = 0;
    processes[processCount].regs.flags = 0x3206; // 0x206 for ring 0
    processes[processCount].regs.esp = fileSizeWithStack - 1;
    processes[processCount].regs.ebp = fileSizeWithStack - 1;
    processes[processCount].regs.eax = 0;
    processes[processCount].regs.ebx = 0;
    processes[processCount].regs.ecx = 0;
    processes[processCount].regs.edx = 0;
    processes[processCount].regs.esi = 0;
    processes[processCount].regs.edi = 0;
    processes[processCount].IPCQueueSize = 0;
    processes[processCount].waiting = false;
    processes[processCount].memStart = allocMem(fileSizeWithStack);
    processes[processCount].pid = tick + processCount;
    serialSendString("[createProcessEntry()]: New process \""); serialSendString(name);
    serialSendString("\", memory allocated at: "); serialSendInt(processes[processCount].memStart); 
    serialSendString(", PID: "); serialSendInt(processes[processCount].pid); serialSendString(", size: ");
    serialSendInt(fileSizeWithStack); serialSend('\n');
    processes[processCount].memSize = fileSizeWithStack;
    processCount++;
    
    return processCount - 1;
}

int getProcessIndexFromPID(uint32_t pid) {
    for(int i = 1; i<processCount; i++) {
        if(processes[i].pid == pid) return i;
    }
    return -1; // Not found
}

int getNextProcessDry() {
    if(processCount == 0) return -1;

    int at = schedulerProcessAt;
    do {
        at++;
        if(at >= processCount) at = 1;
        if(at == 0) at = 1; // Skip the kernel process
    } while(processes[at].waiting);

    return at;
}

int getNextProcess() {
    schedulerProcessAt = getNextProcessDry();
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

uint32_t spawnProcess(const char *path) {
    fileInfo info;
    if(fsGetFileInfo(path, &info) == -1) return 0; // File not found
    uint8_t processBuffer[info.size]; 

    fsReadFile(path, processBuffer, &info);
    if(processBuffer[36] != 'A' && processBuffer[37] != 'E' && processBuffer[38] != 'F') {
        serialSendString("[spawnProcess()]: Invalid process file format\n");
        return 0;
    }

    int processIndex = createProcessEntry((char *)processBuffer, info.size);
    // Get the entry point in the header
    uint32_t *entryPoint = (uint32_t *)(processBuffer + 32);
    processes[processIndex].pcLoc = *entryPoint;

    memcpy((void *)processes[processIndex].memStart, processBuffer, info.size);

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

int sendMessageToProcess(int pid, procMsg *msg) {
    const int idx = getProcessIndexFromPID(pid);
    if(idx == -1) return 0;

    if(processes[idx].IPCQueueSize >= PROCESS_MSG_QUEUE_SIZE) {
        serialSendString("[sendMessageToProcess()]: IPC queue is full\n");
        return 0;
    }

    msg->fromPID = processes[schedulerProcessAt].pid;
    memcpy(&(processes[idx].IPCQueue[processes[idx].IPCQueueSize++]), msg, sizeof(procMsg));
    return 1;
}
