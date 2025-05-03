#include "faultHandlers.h"

__attribute__((interrupt)) void dividErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Division by 0", interruptFrame->ip);
}

__attribute__((interrupt)) void debugErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Debug", interruptFrame->ip);
}

__attribute__((interrupt)) void nmIntErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Non maskable interrupt", interruptFrame->ip);
}

__attribute__((interrupt)) void breakErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Breakpoint", interruptFrame->ip);
}

__attribute__((interrupt)) void overflowErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Overflow", interruptFrame->ip);
}

__attribute__((interrupt)) void boundsCheckErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Bounds check", interruptFrame->ip);
}

__attribute__((interrupt)) void invalidOpcodeErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Invalid opcode", interruptFrame->ip);
}

__attribute__((interrupt)) void devNotAvailErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Device not available", interruptFrame->ip);
}

__attribute__((interrupt)) void doubleFaultErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Double fault", interruptFrame->ip);
}

__attribute__((interrupt)) void coprocSegOverrunErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Coprocessor segment overrun", interruptFrame->ip);
}

__attribute__((interrupt)) void invalidTSSErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Invalid TSS", interruptFrame->ip);
}

__attribute__((interrupt)) void segNotPresentErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Segment not present", interruptFrame->ip);
}

__attribute__((interrupt)) void stackFaultErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Stack fault", interruptFrame->ip);
}

__attribute__((interrupt)) void genProtFaultErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("General protection fault", interruptFrame->ip);
}

__attribute__((interrupt)) void pageFaultErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Page fault", interruptFrame->ip);
}

__attribute__((interrupt)) void reservedErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Unknown (Reserved)", interruptFrame->ip);
}

__attribute__((interrupt)) void fpuErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("FPU Error", interruptFrame->ip);
}

__attribute__((interrupt)) void alignCheckErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Alignment check", interruptFrame->ip);
}

__attribute__((interrupt)) void machineCheckErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("Machine check", interruptFrame->ip);
}

__attribute__((interrupt)) void simdErr(struct interruptFrame *interruptFrame) {
    SET_DS(0x10);
    SET_ES(0x10);
    herr2("SIMD Floating-Point Exception", interruptFrame->ip);
}