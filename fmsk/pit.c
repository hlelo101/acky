#include "pit.h"
#include "io.h"
#include "utils.h"
#include "process.h"
#include "serial.h"
#include "kmain.h"

uint32_t tick = 0;

__attribute__((interrupt)) void PITISR(struct interrupt_frame *interruptFrame) {
    CLI();
    
    if((tick % 10) && canPreempt) { // Switch task
        const int currentProcessIndex = getCurrentProcessIdx();
        const int nextProcessIndex = getNextProcess();
        if(nextProcessIndex == -1) goto exit;

        setProcessPC(currentProcessIndex, (uint32_t)interruptFrame->ip);
        interruptFrame->ip = getPCLoc(nextProcessIndex);
        // interruptFrame->flags |= 0x200; // Set the interrupt flag (is that really needed?)
    }

    exit:
    tick++;
    outb(0x20, 0x20);
    STI();
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