#include "idt.h"
#include "io.h"
#include "ps2kbd.h"
#include "herr.h"
#include "pit.h"
#include "utils.h"
#include "serial.h"
#include "faultHandlers.h"
#include "process.h"

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

__attribute__((naked)) void sysCall(struct interruptFrame *interruptFrame __attribute__((unused))) {
    asm volatile("push %ebx\npush %ecx\npush %edx\npush %eax\n");
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
                serialSendString("[Warning]: Invalid memory access\n");
                syscallReturn = SRET_ERROR;
                break;
            }

            processBufferLoc = (char *)(options.ebx + processes[schedulerProcessAt].memStart); // Convert the process' offset to the real one
            processWaitingForInput = true;
            processes[schedulerProcessAt].waiting = true;
            inputWaintingPID = schedulerProcessAt;

            asm volatile("pop %eax\nmov %eax, 0\npop %edx\npop %ecx\npop %ebx\n"); // I mean yeah you can consider this black magic but... you know, it works
            asm volatile("jmp PITISR");
            break;
        case SC_SPAWNPROC: // Spawn process; EBX
            // EBX = path, ECX = name
            if(options.ebx > (uint32_t)processes[schedulerProcessAt].memSize) {
                serialSendString("[Warning]: Invalid memory access\n");
                syscallReturn = SRET_ERROR;
                break;
            }
            char *path = (char *)(options.ebx + processes[schedulerProcessAt].memStart);
            char *name = (char *)(options.ecx + processes[schedulerProcessAt].memStart);
            FSsetSTI = false;
            const uint32_t newPID = spawnProcess(name, path);
            FSsetSTI = true;
            if(newPID == 0) {
                serialSendString("[Warning]: Failed to spawn process\n");
                syscallReturn = SRET_ERROR;
                break;
            }
            syscallReturn = newPID;
            break;
        case SC_EXIT: // Exit
            kill(processes[schedulerProcessAt].pid);
            asm volatile("pop %eax\nmov %eax, 0\npop %edx\npop %ecx\npop %ebx\n");
            asm volatile("jmp PITISR");
            break;
        case SC_ISPROCRUNNING: // Check if a process is running
            if(getProcessIndexFromPID(options.ebx) == -1) syscallReturn = 0;
            else syscallReturn = 1;
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
        "pop %%eax\n"
        "mov %0, %%eax\n"
        "pop %%edx\n"
        "pop %%ecx\n"
        "pop %%ebx\n"
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
    setIDTGate(0x40, (uint32_t)sysCall, 0x08, 0xEE);        // System calls; flags=0x8E for ring 0
    // Load!
    __asm__ __volatile__ ("lidtl (%0)" :: "r" (&idtr));

    // Enable IRQ1 and IRQ0s
    PICMap(0x20, 0x28, 0xFC, 0xFF);
    asm volatile ("sti");
}