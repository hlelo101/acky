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

int createWindow(const char *name, uint32_t pid) {
    if(windowCount >= MAX_WINDOWS_COUNT) return -1;
    
    strcpy(windows[windowCount].name, name);
    windows[windowCount].processPID = pid;
    windows[windowCount].x = 50;
    windows[windowCount].y = 50;
    windows[windowCount].height = 50;
    windows[windowCount].width = 70;

    return 0;
}
