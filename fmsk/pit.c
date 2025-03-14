#include "pit.h"
#include "io.h"
#include "utils.h"
#include "vga.h"

uint32_t tick = 0;

__attribute__((interrupt)) void PITISR(struct interrupt_frame *interruptFrame __attribute__((unused))) {
    CLI();
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