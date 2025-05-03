#pragma once
#include "io.h"

#define CEIL_DIV(a, b) (((a + b) - 1) / b)
#define CLI() asm volatile("cli")
#define STI() asm volatile("sti")
#define SET_DS(seg) asm volatile ("mov %0, %%ds" : : "r" (seg))
#define SET_CS(seg) asm volatile ("mov %0, %%cs" : : "r" (seg))
#define SET_ES(seg) asm volatile ("mov %0, %%es" : : "r" (seg))
#define bochsConsolePrintChar(c) outb(0xE9, c)