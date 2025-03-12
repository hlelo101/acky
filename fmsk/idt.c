#include "idt.h"
#include "io.h"
#include "ps2kbd.h"
#include "herr.h"
#include "pit.h"

gate idt[256];
idtrDesc idtr;

void PICMap(int masterOffset, int slaveOffset, uint8_t masterMasks, uint8_t slaveMasks) {
    // ICW1
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    // ICW2
    outb(0x21, masterOffset);
    outb(0xA1, slaveOffset);

    // ICW3
    outb(0x21, 4);
    outb(0xA1, 2);

    // ICW4
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // Masks
    outb(0x21, masterMasks);
    outb(0xA1, slaveMasks);
}

void setIDTGate(int index, uint32_t address, uint16_t selector, uint8_t flags) {
    idt[index].offsetLow = address & 0xFFFF;
    idt[index].offsetHigh = (address >> 16) & 0xFFFF;
    idt[index].selector = selector;
    idt[index].zero = 0;
    idt[index].flags = flags;
}

void dividErr() {
    herr("Division by 0");
}

void debugErr() {
    herr("Debug");
}

void nmIntErr() {
    herr("Non maskable interrupt");
}

void breakErr() {
    herr("Breakpoint");
}

void overflowErr() {
    herr("Overflow");
}

void boundsCheckErr() {
    herr("Bounds check");
}

void invalidOpcodeErr() {
    herr("Invalid opcode");
}

void devNotAvailErr() {
    herr("Device not available");
}

void doubleFaultErr() {
    herr("Double fault");
}

void coprocSegOverrunErr() {
    herr("Coprocessor segment overrun");
}

void invalidTSSErr() {
    herr("Invalid TSS");
}

void segNotPresentErr() {
    herr("Segment not present");
}

void stackFaultErr() {
    herr("Stack fault");
}

void genProtFaultErr() {
    herr("General protection fault");
}

void pageFaultErr() {
    herr("Page fault");
}

void reservedErr() {
    herr("Unknown (Reserved)");
}

void fpuErr() {
    herr("FPU Error");
}

void alignCheckErr() {
    herr("Alignment check");
}

void machineCheckErr() {
    herr("Machine check");
}

void simdErr() {
    herr("SIMD Floating-Point Exception");
}

void initIDT() {
    idtr.addr = (uint32_t)idt;
    idtr.size = sizeof(idt) - 1;

    // Exceptions
    setIDTGate(0, (uint32_t)dividErr, 0x08, 0x8F);
    setIDTGate(1, (uint32_t)debugErr, 0x08, 0x8F);
    setIDTGate(2, (uint32_t)nmIntErr, 0x08, 0x8F);
    setIDTGate(3, (uint32_t)breakErr, 0x08, 0x8F);
    setIDTGate(4, (uint32_t)overflowErr, 0x08, 0x8F);
    setIDTGate(5, (uint32_t)boundsCheckErr, 0x08, 0x8F);
    setIDTGate(6, (uint32_t)invalidOpcodeErr, 0x08, 0x8F);
    setIDTGate(7, (uint32_t)devNotAvailErr, 0x08, 0x8F);
    setIDTGate(8, (uint32_t)doubleFaultErr, 0x08, 0x8F);
    setIDTGate(9, (uint32_t)coprocSegOverrunErr, 0x08, 0x8F);
    setIDTGate(10, (uint32_t)invalidTSSErr, 0x08, 0x8F);
    setIDTGate(11, (uint32_t)segNotPresentErr, 0x08, 0x8F);
    setIDTGate(12, (uint32_t)stackFaultErr, 0x08, 0x8F);
    setIDTGate(13, (uint32_t)genProtFaultErr, 0x08, 0x8F);
    setIDTGate(14, (uint32_t)pageFaultErr, 0x08, 0x8F);
    setIDTGate(15, (uint32_t)reservedErr, 0x08, 0x8F);
    setIDTGate(16, (uint32_t)fpuErr, 0x08, 0x8F);
    setIDTGate(17, (uint32_t)alignCheckErr, 0x08, 0x8F);
    setIDTGate(18, (uint32_t)machineCheckErr, 0x08, 0x8F);
    setIDTGate(19, (uint32_t)simdErr, 0x08, 0x8F);
    for(int i = 20; i<=31; i++) setIDTGate(i, (uint32_t)reservedErr, 0x08, 0x8F);

    setIDTGate(0x21, (uint32_t)ps2KBDISR, 0x08, 0x8E);  // Keyboard
    setIDTGate(0x20, (uint32_t)PITISR, 0x08, 0x8E);  // Timer
    // Load!
    __asm__ __volatile__ ("lidtl (%0)" :: "r" (&idtr));

    // Enable IRQ1
    PICMap(0x20, 0x28, 0xFC, 0xFF);
    __asm__ __volatile__ ("sti");
}