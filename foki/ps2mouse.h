#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "io.h"
#include "idt.h"

typedef struct {
    char signature[7]; // "MOUMOV"
    int8_t x;
    int8_t y;
    uint8_t status;
}__attribute__((packed)) mouseMovMsg;

void initPS2Mouse();
__attribute__((interrupt))
void ps2MouseISR(struct interruptFrame *frame __attribute__((unused)));