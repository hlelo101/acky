#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "io.h"
#include "idt.h"

void initPS2Mouse();
__attribute__((interrupt))
void ps2MouseISR(struct interruptFrame *frame __attribute__((unused)));