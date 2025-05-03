#pragma once
#include "utils.h"
#include "idt.h"

__attribute__((interrupt)) void ps2KBDISR(struct interruptFrame *interruptFrame __attribute__((unused)));