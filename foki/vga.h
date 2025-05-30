#pragma once
#define VIDEO_MEM_ADDR 0xB8000
#include <stdint.h>

#define REG_SEQUENCER 0x3C4
#define REG_GRAPHICS 0x3CE
#define REG_CRTC 0x3D4
#define REG_MISC 0x3C2
#define REG_ATTRIBUTE 0x3C0
#define REG_ATTRIBUTE_RESET 0x3DA
#define DATA(reg) ((reg) + 1) // Won't work with the attribute register
#define GRAPHICS_MEM_ADDR 0xA0000
#define PUT_PIXEL(x, y, c) \
    *((uint8_t *)(GRAPHICS_MEM_ADDR + (y) * 320 + (x))) = (c)

void printChar(const char c);
void setColorAttribute(const int attr);
void print(const char *str);
void clearScr();
void printInt(const uint32_t integer);
void updateCursorPos(const unsigned int offset);
void disableCursor();
void enableCursor(uint8_t cursor_start, uint8_t cursor_end);
void intToStr(uint32_t num, char *str);

void init13h();
uint8_t readReg(const int reg, const int index);
void writeReg(const int reg, const int index, const int value);
void drawLine(int x0, int y0, int x1, int y1, uint8_t color);