#pragma once
#include <stdint.h>

#define PIT_CH0_DATA 0x40
#define PIT_CH1_DATA 0x41
#define PIT_CH2_DATA 0x42
#define PIT_COM 0x43

struct interrupt_frame;

void initPIT(const uint32_t freq);
__attribute__((interrupt)) void PITISR(struct interrupt_frame *interruptFrame __attribute__((unused)));