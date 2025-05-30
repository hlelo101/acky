#include "pit.h"
#include "io.h"
#include "utils.h"
#include "process.h"
#include "serial.h"
#include "kmain.h"
#include "gdt.h"

uint32_t tick = 0;
bool saveRegs = true;

__attribute__((interrupt))
void PITISR(struct interrupt_frame *interruptFrame) {
    asm volatile("push %esi\npush %edi\npush %ebx\npush %ecx\npush %edx\npush %eax\n");
    SET_DS(0x10);
    
    if(canPreempt) { // Switch task
        const int currentProcessIndex = getCurrentProcessIdx();
        const int nextProcessIndex = getNextProcess();
        if(nextProcessIndex == -1) goto trampolineSetup;

        if(saveRegs) {
            uint32_t *esp;
            asm volatile("mov %%esp, %0" : "=r"(esp));

            processes[currentProcessIndex].regs.eax = esp[0];
            processes[currentProcessIndex].regs.edx = esp[1];
            processes[currentProcessIndex].regs.ecx = esp[2];
            processes[currentProcessIndex].regs.ebx = esp[3];
            processes[currentProcessIndex].regs.edi = esp[4];
            processes[currentProcessIndex].regs.esi = esp[5];
            processes[currentProcessIndex].regs.esp = interruptFrame->sp;
            processes[currentProcessIndex].regs.flags = interruptFrame->flags;

            setProcessPC(currentProcessIndex, (uint32_t)interruptFrame->ip);
        } else saveRegs = true;
        // serialSendString("[PITISR]: Saved EBP: "); serialSendInt(processes[currentProcessIndex].regs.ebp); serialSendString(" for process ");
        // serialSendString(processes[currentProcessIndex].name); serialSend('\n');
        
        uint32_t processMemSizeWithStack = processes[nextProcessIndex].memSize + (4096 * 4 + 1);
        setLDTEntry(0, processes[nextProcessIndex].memStart, processMemSizeWithStack, 0xFA, 0xCF);
        setLDTEntry(1, processes[nextProcessIndex].memStart, processMemSizeWithStack, 0xF2, 0xCF);
        asm volatile("lldt %0" : : "r"(0x1B)); // Reload the LDT to avoid some weird CPU shittery; selector 0x18 for ring 0

        // Restore the CPU state
        proc_regs_eax = processes[nextProcessIndex].regs.eax;
        proc_regs_edx = processes[nextProcessIndex].regs.edx;
        proc_regs_ecx = processes[nextProcessIndex].regs.ecx;
        proc_regs_ebx = processes[nextProcessIndex].regs.ebx;
        proc_regs_esp = processes[nextProcessIndex].regs.esp;
        proc_regs_esi = processes[nextProcessIndex].regs.esi;
        proc_regs_edi = processes[nextProcessIndex].regs.edi;
        proc_flags = processes[nextProcessIndex].regs.flags;
        proc_ip = processes[nextProcessIndex].pcLoc;
        proc_regs_ebp = processes[nextProcessIndex].regs.ebp;
        
        trampolineSetup:
        interruptFrame->ip = (uint32_t)trampoline;
        asm volatile(
            "pushf\n"
            "pop %0\n"
            : "=r"(interruptFrame->flags) : : "memory" 
        );
        interruptFrame->flags &= ~(1 << 9); // Do not re-enable interrupts

        interruptFrame->cs = 0x08;
        interruptFrame->ss = 0x10;
        interruptFrame->sp = processes[nextProcessIndex].regs.ebp;
    }

    tick++;
    asm volatile("pop %eax\npop %edx\npop %ecx\npop %ebx\npop %edi\npop %esi\n");
    outb(0x20, 0x20);
    if(!canPreempt && systemInitialized) SET_DS(0x0C);
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
