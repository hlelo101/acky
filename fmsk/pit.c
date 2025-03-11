#include "pit.h"
#include "io.h"
#include "vga.h"

__attribute__((interrupt)) void PITISR(struct interrupt_frame *interruptFrame __attribute__((unused))) {
    // More stuff will be implemented here for things like preemptive multitasking
    outb(0x20, 0x20);
}

void initPIT(const uint32_t freq) {
    uint32_t divisor = 1193180 / freq;

    outb(0x43,0x36);
    outb(0x40,(uint8_t)(divisor & 0xFF));
    outb(0x40,(uint8_t)((divisor >> 8) & 0xFF));
}