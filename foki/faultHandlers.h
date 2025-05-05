#pragma once
#include <stdint.h>
#include "idt.h"
#include "utils.h"
#include "herr.h"

__attribute__((interrupt)) void dividErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void debugErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void nmIntErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void breakErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void overflowErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void boundsCheckErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void invalidOpcodeErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void devNotAvailErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void doubleFaultErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void coprocSegOverrunErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void invalidTSSErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void segNotPresentErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void stackFaultErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void genProtFaultErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void pageFaultErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void reservedErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void fpuErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void alignCheckErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void machineCheckErr(struct interruptFrame *interruptFrame);
__attribute__((interrupt)) void simdErr(struct interruptFrame *interruptFrame);