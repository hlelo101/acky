#pragma once
#include <stdint.h>

#define MAX_WINDOWS_COUNT 255

typedef struct {
    char name[10];
    uint16_t x;
    uint16_t y;
    uint16_t width;     // This is the size of the contents of the window, doesn't include the
    uint16_t height;    // title bar, borders, etc
    uint32_t processPID;
} window;
