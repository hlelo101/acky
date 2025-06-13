#pragma once
#include <stdint.h>
#include <acky.h>
#include <stdbool.h>
#include "img.h"

#define MAX_WINDOWS_COUNT 255
#define TITLEBAR_COLOR 22, 186, 159

typedef struct {
    char name[20];
    uint16_t x;
    uint16_t y;
    uint16_t width;     // This is the size of the contents of the window, doesn't include the
    uint16_t height;    // title bar, borders, etc
    uint32_t processPID;
    bool titleBar;
} window;

extern uint8_t imageData[];
extern window windows[];

void drawWindow(int idx);
int wmCreateWindow(const char *name, uint16_t x, uint16_t y, uint16_t height, uint16_t width, bool titleBar, uint32_t pid);
int checkWindowHover(uint16_t x, uint16_t y);
int getIndexFromPID(uint32_t pid);
