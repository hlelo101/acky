#include "faultHandlers.h"

__attribute__((interrupt)) void dividErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Division by 0", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void debugErr(struct interruptFrame *interruptFrame __attribute__((unused))) {
    SET_DS(0x10);
    SET_ES(0x10);
    print("Debug!\n");
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void nmIntErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Non maskable interrupt", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void breakErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Breakpoint", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void overflowErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Overflow", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void boundsCheckErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Bounds check", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void invalidOpcodeErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Invalid opcode", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void devNotAvailErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Device not available", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void doubleFaultErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Double fault", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void coprocSegOverrunErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Coprocessor segment overrun", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void invalidTSSErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Invalid TSS", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void segNotPresentErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Segment not present", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void stackFaultErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Stack fault", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void genProtFaultErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("General protection fault", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void pageFaultErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Page fault", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void reservedErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Unknown (Reserved)", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void fpuErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("FPU Error", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void alignCheckErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Alignment check", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void machineCheckErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("Machine check", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}

__attribute__((interrupt)) void simdErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    intHerr("SIMD Floating-Point Exception", interruptFrame);
    SET_DS(0x0F);
    SET_ES(0x0F);
}