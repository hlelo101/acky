#include "pit.h"
#include "io.h"
#include "utils.h"
#include "vga.h"

__attribute__((interrupt)) void PITISR(struct interrupt_frame *interruptFrame __attribute__((unused))) {
    CLI();
    // More stuff will be implemented here for things like preemptive multitasking

    // Get the PIT count
    outb(0x43,0b0000000);
	int count = inb(0x40);
	count |= inb(0x40)<<8;

    outb(0x20, 0x20);
    STI();
}

void initPIT(const uint32_t freq) {
    uint32_t divisor = 1193180 / freq;

    outb(0x43,0x36);
    outb(0x40,(uint8_t)(divisor & 0xFF));
    outb(0x40,(uint8_t)((divisor >> 8) & 0xFF));
}