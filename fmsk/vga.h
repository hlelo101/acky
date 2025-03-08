#pragma once
#define VIDEO_MEM_ADDR 0xC00B8000
#include <stdint.h>

void printChar(const char c);
void setColorAttribute(const int attr);
void print(const char *str);
void clearScr();
void printInt(const int integer);
void updateCursorPos(const unsigned int offset);
void disableCursor();
void enableCursor(uint8_t cursor_start, uint8_t cursor_end);