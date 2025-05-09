#pragma once
#include "vga.h"
#include "idt.h"
#include "process.h"

#define HERR_COLOR_ATTRIBUTE 0x4C

void herr(const char *str);
void intHerr(const char *str, struct interruptFrame *interruptFrame);