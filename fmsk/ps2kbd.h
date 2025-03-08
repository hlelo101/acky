#pragma once

struct interrupt_frame; // Shut up GCC

__attribute__((interrupt)) void ps2KBDISR(struct interrupt_frame *interruptFrame __attribute__((unused)));