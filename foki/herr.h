#pragma once
#include "vga.h"
#include "idt.h"
#include "process.h"

void herr(const char *str);
void intHerr(const char *str, struct interruptFrame *interruptFrame);