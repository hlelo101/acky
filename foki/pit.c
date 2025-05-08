#include "pit.h"
#include "io.h"
#include "utils.h"
#include "process.h"
#include "serial.h"
#include "kmain.h"
#include "gdt.h"

uint32_t tick = 0;

__attribute__((interrupt)) void PITISR(struct interrupt_frame *interruptFrame) {
    asm volatile("push %ebp\npush %ebx\npush %ecx\npush %edx\npush %eax\n");
    SET_DS(0x10);
    
    if(canPreempt) { // Switch task
        const int currentProcessIndex = getCurrentProcessIdx();

        const int nextProcessIndex = getNextProcess();
        if(nextProcessIndex == -1) { // Error occured 3:
            proc_ip = interruptFrame->ip;
            proc_flags = interruptFrame->flags;
            interruptFrame->ip = (uint32_t)trampoline;
            interruptFrame->flags &= ~(1 << 9); // Do not re-enable interrupts
            interruptFrame->cs = 0x08;
            interruptFrame->ss = 0x10;
            interruptFrame->sp = realTSS.esp0;
            goto exit;
        }
        uint32_t *esp;
        asm volatile("mov %%esp, %0" : "=r"(esp));

        processes[currentProcessIndex].regs.eax = esp[0];
        processes[currentProcessIndex].regs.edx = esp[1];
        processes[currentProcessIndex].regs.ecx = esp[2];
        processes[currentProcessIndex].regs.ebx = esp[3];
        // processes[currentProcessIndex].regs.ebp = esp[4]; The reason why we do not save EBP is because
        // we'll just put the wrong value and the process has no reason to change it...
        processes[currentProcessIndex].regs.esp = interruptFrame->sp;
        processes[currentProcessIndex].regs.flags = interruptFrame->flags;
        // serialSendString("Got IP: "); serialSendInt(interruptFrame->ip);
        // serialSendString(" for process "); serialSendString(processes[currentProcessIndex].name); serialSend('\n');
        
        setProcessPC(currentProcessIndex, (uint32_t)interruptFrame->ip);
        interruptFrame->ip = (uint32_t)trampoline;
        
        setLDTEntry(0, processes[nextProcessIndex].memStart, 0x1000, 0xFA, 0xCF);
        setLDTEntry(1, processes[nextProcessIndex].memStart, 0x1000, 0xF2, 0xCF);
        asm volatile("lldt %0" : : "r"(0x1B)); // Reload the LDT to avoid some weird CPU shittery; selector 0x18 for ring 0
        
        // Restore the CPU state
        proc_regs_eax = processes[nextProcessIndex].regs.eax;
        proc_regs_edx = processes[nextProcessIndex].regs.edx;
        proc_regs_ecx = processes[nextProcessIndex].regs.ecx;
        proc_regs_ebx = processes[nextProcessIndex].regs.ebx;
        proc_regs_esp = processes[nextProcessIndex].regs.esp;
        proc_flags = processes[nextProcessIndex].regs.flags;
        proc_ip = processes[nextProcessIndex].pcLoc;
        // serialSendString("Setting EBP to: "); serialSendInt(processes[nextProcessIndex].regs.ebp); serialSend('\n');
        proc_regs_ebp = processes[nextProcessIndex].regs.ebp;
        // LDT stuff
        // interruptFrame->cs = 0x04;
        interruptFrame->flags &= ~(1 << 9); // Do not re-enable interrupts

        interruptFrame->cs = 0x08;
        interruptFrame->ss = 0x10;
        interruptFrame->sp = realTSS.esp0;
    }

    exit:
    tick++;
    asm volatile("pop %eax\npop %edx\npop %ecx\npop %ebx\npop %ebp\n");
    outb(0x20, 0x20);
    if(!canPreempt) SET_DS(0x0C);
}

void sleep(uint32_t t) {
    uint32_t start = tick;
    while(tick - start < t);
}

void initPIT(const uint32_t freq) {
    uint32_t divisor = 1193180 / freq;

    outb(0x43,0x36);
    outb(0x40,(uint8_t)(divisor & 0xFF));
    outb(0x40,(uint8_t)((divisor >> 8) & 0xFF));
}