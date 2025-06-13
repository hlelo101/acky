#include "window.h"

window windows[MAX_WINDOWS_COUNT];
uint8_t windowCount;

void strcpy(char *dest, const char *src) {
    while(*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = 0;
}

int wmCreateWindow(const char *name, uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool titleBar, uint32_t pid) {
    if(windowCount >= MAX_WINDOWS_COUNT) return -1;
    
    strcpy(windows[windowCount].name, name);
    windows[windowCount].processPID = pid;
    windows[windowCount].x = x;
    windows[windowCount].y = y;
    windows[windowCount].height = height;
    windows[windowCount].width = width;
    windows[windowCount].titleBar = titleBar;

    return windowCount++;
}

void drawWindow(int idx) {
    if(idx >= windowCount) return;
    if(windows[idx].titleBar) {
        drawSquare(
            windows[idx].x, windows[idx].y - 12,
            windows[idx].width, 12, TITLEBAR_COLOR
        );
        renderStr(windows[idx].name, imageData, windows[idx].x + 2, windows[idx].y - 10, 0, 0, 0);
    }
    drawSquare(
        windows[idx].x, windows[idx].y,
        windows[idx].width, windows[idx].height,
        54, 54, 54
    );
}

int checkWindowHover(uint16_t x, uint16_t y) {
    for(int i = 0; i < windowCount; i++) {
        if(
            x >= windows[i].x && x <= windows[i].x + windows[i].width &&
            y >= windows[i].y - 12 && y <= windows[i].y
        ) return i;
    }

    return -1;
}

int getIndexFromPID(uint32_t pid) {
    for(int i = 0; i < windowCount; i++) {
        if(windows[i].processPID == pid) return i;
    }

    return -1;
}
