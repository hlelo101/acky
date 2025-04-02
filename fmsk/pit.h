#pragma once
#include <stdint.h>

#define PIT_CH0_DATA 0x40
#define PIT_CH1_DATA 0x41
#define PIT_CH2_DATA 0x42
#define PIT_COM 0x43

struct interrupt_frame {
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t sp;
    uint32_t ss;
};

void initPIT(const uint32_t freq);
__attribute__((interrupt)) void PITISR(struct interrupt_frame *interruptFrame);
void sleep(uint32_t t);