#include "idt.h"
#include "io.h"
#include "ps2kbd.h"
#include "herr.h"
#include "pit.h"
#include "utils.h"
#include "serial.h"
#include "faultHandlers.h"
#include "process.h"
#include "gdt.h"
#include "acpi.h"
#include "fs.h"
#include "ps2mouse.h"

gate idt[256];
idtrDesc idtr;

void PICMap(int masterOffset, int slaveOffset, uint8_t masterMasks, uint8_t slaveMasks) {
    // ICW1
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    // ICW2
    outb(0x21, masterOffset);
    outb(0xA1, slaveOffset);

    // ICW3
    outb(0x21, 4);
    outb(0xA1, 2);

    // ICW4
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // Masks
    outb(0x21, masterMasks);
    outb(0xA1, slaveMasks);
}

void setIDTGate(int index, uint32_t address, uint16_t selector, uint8_t flags) {
    idt[index].offsetLow = address & 0xFFFF;
    idt[index].offsetHigh = (address >> 16) & 0xFFFF;
    idt[index].selector = selector;
    idt[index].zero = 0;
    idt[index].flags = flags;
}

// That weird IRQ7
__attribute__((interrupt)) void IRQ7(struct interrupt_frame *interruptFrame __attribute__((unused))) {
    outb(0x20, 0x0B);
    unsigned char irr = inb(0x20);
    if(!(irr & 0x80)) return;

    outb(0xA0, 0x0B);
    outb(0x20, 0x0B);

    SET_DS(0x0F);
}

void wait() {
    while(1);
}

int pX, pY, sX, sY;
uint8_t color;
__attribute__((naked)) void sysCall(struct interruptFrame *interruptFrame __attribute__((unused))) {
    asm volatile("push %esi\npush %edi\npush %ebp\npush %ebx\npush %ecx\npush %edx\npush %eax\n");
    asm volatile("push %eax");
    SET_DS(0x10);
    SET_ES(0x10);
    asm volatile("pop %eax");
    uint32_t syscallReturn = SRET_SUCCESS;

    // Get the register contents
    generalRegs options;
    asm volatile("mov %%eax, %0" : "=r"(options.eax));
    asm volatile("mov %%ebx, %0" : "=r"(options.ebx));
    asm volatile("mov %%ecx, %0" : "=r"(options.ecx));
    asm volatile("mov %%edx, %0" : "=r"(options.edx));

    switch(options.eax) {
        case SC_PRINTCHAR: // Print character
            char c = (char)(options.ebx & 0xFF);
            printChar(c);
            break;
        case SC_GETUINPUT: // Get user input
            // Check the buffer (address in EBX)
            if(options.ebx > (uint32_t)processes[schedulerProcessAt].memSize) {
                serialSendString("[SC_GETUINPUT]: Invalid memory access\n");
                syscallReturn = SRET_ERROR;
                break;
            }

            if(options.ecx <= 0) {
                serialSendString("[SC_GETUINPUT]: Invalid buffer size");
            }

            processBufferLoc = (char *)(options.ebx + processes[schedulerProcessAt].memStart); // Convert the process' offset to the real one
            processBufferSize = options.ecx;
            processWaitingForInput = true;
            processes[schedulerProcessAt].waiting = true;
            inputWaintingPID = schedulerProcessAt;

            asm volatile("pop %eax\npop %edx\npop %ecx\npop %ebx\npop %ebp\npop %edi\npop %esi\n"); // I mean yeah you can consider this black magic but... you know, it works
            asm volatile("jmp PITISR");
            break;
        case SC_SPAWNPROC: // Spawn process; EBX
            // EBX = path
            if(options.ebx > (uint32_t)processes[schedulerProcessAt].memSize) {
                serialSendString("[SC_SPAWNPROC]: Invalid memory access\n");
                syscallReturn = SRET_ERROR;
                break;
            }
            char *path = (char *)(options.ebx + processes[schedulerProcessAt].memStart);
            serialSendString("[SC_SPAWNPROC]: Got path: "); serialSendString(path); serialSend('\n');
            FSsetSTI = false;
            const uint32_t newPID = spawnProcess(path);
            FSsetSTI = true;
            if(newPID == 0) {
                serialSendString("[SC_SPAWNPROC]: Failed to spawn process\n");
                syscallReturn = SRET_ERROR;
                break;
            }
            syscallReturn = newPID;
            break;
        case SC_EXIT: // Exit
            kill(processes[schedulerProcessAt].pid);
            saveRegs = false;

            setLDTEntry(0, (uint32_t)wait, 0x1000, 0xFA, 0xCF);
            setLDTEntry(1, (uint32_t)wait, 0x1000, 0xF2, 0xCF);
            asm volatile("lldt %0" : : "r"(0x1B)); // We do the same thing as the PITISR :3
            
            asm volatile(
                "mov %ebp, %esp\n"
                "push $0x0F\n"
                "push %esp\n"
                "pushl $0x3206\n"
                "push $0x07\n"
                "pushl $0\n"
                "iret\n"
            );
            break;
        case SC_ISPROCRUNNING: // Check if a process is running
            if(getProcessIndexFromPID(options.ebx) == -1) syscallReturn = 0;
            else syscallReturn = 1;
            break;
        case SC_CLEARSCR:
            clearScr();
            break;
        case SC_GETPORCNUM:
            syscallReturn = processCount;
            break;
        case SC_GETPROCINFO:
            if((options.ebx > (uint32_t)processes[schedulerProcessAt].memSize)) {
                serialSendString("[SC_GETPROCINFO]: Invalid memory access\n");
                syscallReturn = SRET_ERROR;
                break;
            }
            const int procIdx = getProcessIndexFromPID(options.ecx);
            if(procIdx == -1) {
                serialSendString("[SC_GETPROCINFO]: Process not found\n");
                syscallReturn = SRET_ERROR;
                break;
            }

            userProcInfo *info = (userProcInfo *)(options.ebx + processes[schedulerProcessAt].memStart);
            info->pid = processes[procIdx].pid;
            info->memUsage = processes[procIdx].memSize;
            strcpy(info->name, processes[procIdx].name);
            break;
        case SC_GETPROCPIDFROMIDX:
            if(options.ebx >= (uint32_t)processCount) {
                serialSendString("[SC_GETPROCPIDFROMIDX]: Invalid process index\n");
                syscallReturn = SRET_ERROR;
                break;
            }
            syscallReturn = processes[options.ebx].pid;
            break;
        case SC_SERIALSEND:
            char serialC = (char)(options.ebx & 0xFF);
            serialSend(serialC);
            break;
        case SC_CHPWR:
            if(options.ebx == 0) shutdown();
            else if(options.ebx == 1) reboot();
            break;
        case SC_MSG: // EBX = 0: Send message, EBX = 1: Pop message; ECX = Struct address; EDX = Receiver PID
            if(options.ebx == 0) {
                const int idx = getProcessIndexFromPID(options.edx);
                if(idx == -1) {
                    serialSendString("[SC_MSG]: Invalid receiver PID\n");
                    syscallReturn = SRET_ERROR;
                    break;
                }
                if(options.ecx > (uint32_t)processes[schedulerProcessAt].memSize) {
                    serialSendString("[SC_MSG]: Invalid memory access\n");
                    syscallReturn = SRET_ERROR;
                    break;
                }
                procMsg *msg = (procMsg *)(options.ecx + processes[schedulerProcessAt].memStart);
                if(!sendMessageToProcess(processes[schedulerProcessAt].pid, msg)) {
                    serialSendString("[SC_MSG]: Failed to send message\n");
                    syscallReturn = SRET_ERROR;
                    break;
                }
                memcpy(&(processes[idx].IPCQueue[processes[idx].IPCQueueSize++]), msg, sizeof(procMsg));
            } else if(options.ebx == 1) {
                if(options.ecx > (uint32_t)processes[schedulerProcessAt].memSize) {
                    serialSendString("[SC_MSG]: Invalid memory access\n");
                    syscallReturn = SRET_ERROR;
                    break;
                }
                procMsg *cpyTo = (procMsg *)(options.ecx + processes[schedulerProcessAt].memStart);
                if(processes[schedulerProcessAt].IPCQueueSize <= 0) {
                    // We don't print anything because processes will check for new messages in a loop
                    syscallReturn = SRET_ERROR;
                    break;
                }
                cpyTo->fromPID = processes[schedulerProcessAt].IPCQueue[0].fromPID;
                memcpy(cpyTo->msg, processes[schedulerProcessAt].IPCQueue[0].msg, sizeof(cpyTo->msg));
                // Shift the queue
                for(int i = 0; i < processes[schedulerProcessAt].IPCQueueSize - 1; i++) {
                    memcpy(&(processes[schedulerProcessAt].IPCQueue[i]), &(processes[schedulerProcessAt].IPCQueue[i + 1]), sizeof(procMsg));
                }
                processes[schedulerProcessAt].IPCQueueSize--;
            }
            break;
        case SC_CHLAYOUT: // Change keyboard layout; EBX = 0: FR, EBX = 1: UK
            if(options.ebx > 1) {
                syscallReturn = SRET_ERROR;
                break;
            }
            layout = (options.ebx == 0) ? false : true;
            break;
        case SC_GRAPHICS:
            // EBX:
            // 0: Set 13h mode; 1: Set primary X Y; 2: Set secondary X Y; 3: Set color; 4: Put a pixel; 5: Draw a line; 6: Get screen ownership; 7: Get pixel
            switch(options.ebx) {
                case 0:
                    init13h();
                    break;
                case 1:
                    if(options.ecx > 320 || options.edx > 200) {
                        serialSendString("[SC_GRAPHICS, 1]: Invalid coordinates\n");
                        syscallReturn = SRET_ERROR;
                        break;
                    }
                    pX = options.ecx;
                    pY = options.edx;
                    break;
                case 2:
                    if(options.ecx > 320 || options.edx > 200) {
                        serialSendString("[SC_GRAPHICS, 2]: Invalid coordinates\n");
                        syscallReturn = SRET_ERROR;
                        break;
                    }
                    sX = options.ecx;
                    sY = options.edx;
                    break;
                case 3:
                    if(options.ecx > 15) {
                        serialSendString("[SC_GRAPHICS, 3]: Invalid color\n");
                        syscallReturn = SRET_ERROR;
                        break;
                    }
                    color = (uint8_t)options.ecx;
                    break;
                case 4:
                    if(options.ecx > 320 || options.edx > 200) {
                        // serialSendString("[SC_GRAPHICS, 4]: Invalid coordinates, got: ");
                        // serialSendInt(options.ecx); serialSendString(", "); serialSendInt(options.edx);
                        // serialSend('\n');
                        syscallReturn = SRET_ERROR;
                        break;
                    }
                    PUT_PIXEL(options.ecx, options.edx, color);
                    break;
                case 5:
                    drawLine(pX, pY, sX, sY, color);
                    break;
                case 6:
                    ownerPID = processes[schedulerProcessAt].pid;
                    serialSendString("[SC_GRAPHICS, 6]: Screen ownership set to PID ");
                    serialSendInt(ownerPID); serialSend('\n');
                    break;
                case 7:
                    if(options.ecx > 320 || options.edx > 200) {
                        // serialSendString("[SC_GRAPHICS, 7]: Invalid coordinates\n");
                        syscallReturn = SRET_ERROR;
                        break;
                    }
                    syscallReturn = GET_PIXEL(options.ecx, options.edx);
                    break;
                default:
                    serialSendString("[SC_GRAPHICS]: Invalid option\n");
                    syscallReturn = SRET_ERROR;
                    break;
            }
            break;
        case SC_LOADFILE: // Load file; EBX = Path, ECX = Buffer address
            if(options.ebx > (uint32_t)processes[schedulerProcessAt].memSize || options.ecx > (uint32_t)processes[schedulerProcessAt].memSize) {
                serialSendString("[SC_LOADFILE]: Invalid memory access\n");
                syscallReturn = SRET_ERROR;
                break;
            }
            char *filePath = (char *)(options.ebx + processes[schedulerProcessAt].memStart);
            uint8_t *buffer = (uint8_t *)(options.ecx + processes[schedulerProcessAt].memStart);
            fileInfo tInfo;
            FSsetSTI = false;
            serialSendString("[SC_LOADFILE]: Got path \""); serialSendString(filePath);
            serialSendString("\"\n");
            if(fsReadFile(filePath, buffer, &tInfo) == -1) {
                serialSendString("[SC_LOADFILE]: Failed to load file\n");
                syscallReturn = SRET_ERROR;
                FSsetSTI = true;
                break;
            }
            FSsetSTI = true;
            break;
        case SC_GETFILEINFO: // Get file info, EBX = Path, ECX = Struct address
            if(options.ebx > (uint32_t)processes[schedulerProcessAt].memSize || options.ecx > (uint32_t)processes[schedulerProcessAt].memSize) {
                serialSendString("[SC_GETFILEINFO]: Invalid memory access\n");
                syscallReturn = SRET_ERROR;
                break;
            }
            char *filePathInfo = (char *)(options.ebx + processes[schedulerProcessAt].memStart);
            fileInfo *fInfo = (fileInfo *)(options.ecx + processes[schedulerProcessAt].memStart);
            FSsetSTI = false;
            if(fsGetFileInfo(filePathInfo, fInfo) == -1) {
                serialSendString("[SC_GETFILEINFO]: Failed to get file info\n");
                syscallReturn = SRET_ERROR;
                FSsetSTI = true;
                break;
            }
            FSsetSTI = true;
            break;
        default:
            // Invalid syscall
            serialSendString("[Warning]: Invalid system call\n");
            syscallReturn = SRET_ERROR;
            break;
    }
    SET_DS(0x0F);
    SET_ES(0x0F);
    asm volatile(
        "pop %%eax\nmov %0, %%eax\npop %%edx\npop %%ecx\npop %%ebx\npop %%ebp\npop %%edi\npop %%esi\n"
        "iret"
        :
        : "r"(syscallReturn)
        : "eax", "edx", "ecx", "ebx"
    );
}

void setTrampoline(struct interruptFrame *interruptFrame) {
    proc_ip = interruptFrame->ip;
    proc_flags = interruptFrame->flags;
    interruptFrame->ip = (uint32_t)trampoline;
    interruptFrame->flags &= ~(1 << 9);
}

void initIDT() {
    idtr.addr = (uint32_t)idt;
    idtr.size = sizeof(idt) - 1;

    // Exceptions
    setIDTGate(0, (uint32_t)dividErr, 0x08, 0x8F);
    setIDTGate(1, (uint32_t)debugErr, 0x08, 0x8F);
    setIDTGate(2, (uint32_t)nmIntErr, 0x08, 0x8F);
    setIDTGate(3, (uint32_t)breakErr, 0x08, 0x8F);
    setIDTGate(4, (uint32_t)overflowErr, 0x08, 0x8F);
    setIDTGate(5, (uint32_t)boundsCheckErr, 0x08, 0x8F);
    setIDTGate(6, (uint32_t)invalidOpcodeErr, 0x08, 0x8F);
    setIDTGate(7, (uint32_t)devNotAvailErr, 0x08, 0x8F);
    setIDTGate(8, (uint32_t)doubleFaultErr, 0x08, 0x8F);
    setIDTGate(9, (uint32_t)coprocSegOverrunErr, 0x08, 0x8F);
    setIDTGate(10, (uint32_t)invalidTSSErr, 0x08, 0x8F);
    setIDTGate(11, (uint32_t)segNotPresentErr, 0x08, 0x8F);
    setIDTGate(12, (uint32_t)stackFaultErr, 0x08, 0x8F);
    setIDTGate(13, (uint32_t)genProtFaultErr, 0x08, 0x8F);
    setIDTGate(14, (uint32_t)pageFaultErr, 0x08, 0x8F);
    setIDTGate(15, (uint32_t)reservedErr, 0x08, 0x8F);
    setIDTGate(16, (uint32_t)fpuErr, 0x08, 0x8F);
    setIDTGate(17, (uint32_t)alignCheckErr, 0x08, 0x8F);
    setIDTGate(18, (uint32_t)machineCheckErr, 0x08, 0x8F);
    setIDTGate(19, (uint32_t)simdErr, 0x08, 0x8F);
    for(int i = 20; i<=31; i++) setIDTGate(i, (uint32_t)reservedErr, 0x08, 0x8F);

    setIDTGate(0x21, (uint32_t)ps2KBDISR, 0x08, 0x8E);      // Keyboard
    setIDTGate(0x20, (uint32_t)PITISR, 0x08, 0x8E);         // Timer
    setIDTGate(0x27, (uint32_t)IRQ7, 0x08, 0xEE);           // Fucking IRQ7 that triggers for no fucking reason why the fuck are you redundant you stupid motherfu-
    setIDTGate(0x2C, (uint32_t)ps2MouseISR, 0x08, 0x8E);    // PS/2 Mouse
    setIDTGate(0x40, (uint32_t)sysCall, 0x08, 0xEE);        // System calls; flags=0x8E for ring 0
    // Load!
    __asm__ __volatile__ ("lidtl (%0)" :: "r" (&idtr));

    // Enable some IRQs and initialize the PIC
    PICMap(0x20, 0x28, 0xF8, 0xEF);
    asm volatile("sti");
}
