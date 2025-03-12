#pragma once

#define CEIL_DIV(a, b) (((a + b) - 1) / b)
#define CLI() asm volatile("cli")
#define STI() asm volatile("sti")