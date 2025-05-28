#pragma once
#include <stdbool.h>
#include "utils.h"
#include "idt.h"
#include "io.h"
#include "vga.h"
#include "process.h"

extern char *processBufferLoc;
extern bool processWaitingForInput;
extern int inputWaintingPID;
extern int processBufferSize;
extern bool layout;

__attribute__((interrupt)) void ps2KBDISR(struct interruptFrame *interruptFrame __attribute__((unused)));