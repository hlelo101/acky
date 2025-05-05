#pragma once
#include <stdbool.h>
#include "io.h"

#define COM1 0x3F8

void initSerial();
void serialSend(char c);
void serialSendString(const char* str);
void serialSendInt(int num);