#include <acky.h>
#include "img.h"

#define CLEAR() for(int i = 0; i < 320 * 200; i++) { putPixel(i % 320, i / 320); }
#define WM_BG_COLOR 3

uint8_t imageData[2500], mouseData[2048], mouseBG[4 * 4] = {WM_BG_COLOR};
int mouseX = 50, mouseY = 50;

// Some utilities because we still don't have a stdlib
int strcmp(const char *s1, const char *s2) {
    while(*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

void intToStr(int N, char *str) {
    int i = 0;

    int sign = N;

    if(N < 0)
        N = -N;

    while(N > 0) {
        str[i++] = N % 10 + '0';
      	N /= 10;
    } 

    if(sign < 0) {
        str[i++] = '-';
    }

    str[i] = '\0';

    for(int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

typedef struct {
    char signature[7]; // "MOUMOV"
    int8_t x;
    int8_t y;
    uint8_t status;
}__attribute__((packed)) mouseMovMsg;

void checkPower() {
    // The power button's resolution is 15 × 20
    // Gets drawn at offset 3
    if((mouseX >= 3 && mouseX <= 3 + 15) && (mouseY >= 3 && mouseY <= 3 + 20)) {
        shutdown();
    }
}

void main() {
    getScreenOwnership(); // Used to indicate to the kernel where to redirect mouse events, etc
    enable13hMode();

    setColor(WM_BG_COLOR);
    CLEAR();

    // Background
    setColor(1);
    if(loadFile("A:/PROGDAT/WM/BACK.BMP", imageData) == SRET_ERROR) {
        serialPrint("An error occurred while loading \"A:/PROGDAT/WM/BACK.BMP\"");
        while(1);
    }

    // Power
    parseBMP(imageData, 60, 150);
    if(loadFile("A:/PROGDAT/WM/POWER.BMP", imageData) == SRET_ERROR) {
        serialPrint("An error occurred while loading \"A:/PROGDAT/WM/POWER.BMP\"");
        while(1);
    }
    setColor(4);
    parseBMP(imageData, 3, 3);
    drawBorder(1, 1, 18, 23, 6);
    
    // Mouse
    setColor(0);
    if(loadFile("A:/PROGDAT/WM/MOUSE2.BMP", mouseData) == SRET_ERROR) { // A:/PROGDAT/WM/MOUSESMA.BMP
        serialPrint("An error occurred while loading \"A:/PROGDAT/MOUSE2.BMP\"");
        while(1);
    }
    parseBMP(mouseData, mouseX, mouseY);

    procMsg msg;
    mouseMovMsg *mouseMsg;
    while(1) {
        if(popMsg(&msg) == SRET_SUCCESS) {
            mouseMsg = (mouseMovMsg *)msg.msg;
            if(strcmp(mouseMsg->signature, "MOUMOV") == 0) {
                // Draw the mouse background
                for(int i = 0; i < 4; i++) {
                    for(int j = 0; j < 4; j++) {
                        setColor(mouseBG[i * 4 + j]);
                        putPixel(mouseX + j, mouseY + i);
                    }
                }
                mouseX += mouseMsg->x;
                mouseY -= mouseMsg->y;
                if(mouseX < 0) mouseX = 0;
                if(mouseY < 0) mouseY = 0;
                if(mouseX > 320 - 1) mouseX = 320 - 1;
                if(mouseY > 200 - 1) mouseY = 200 - 1;

                // Save the background
                for(int i = 0; i < 4; i++) {
                    for(int j = 0; j < 4; j++) {
                        mouseBG[i * 4 + j] = getPixel(mouseX + j, mouseY + i);
                    }
                }

                uint8_t color = 0;
                if(mouseMsg->status & 0x01) {
                    color = 15;
                    checkPower();
                }

                if(mouseMsg->status & 0x02) color = 14; 
                
                // Draw the mouse
                setColor(color);
                parseBMP(mouseData, mouseX, mouseY);
            } else {
                serialPrint("Unknown message received\n");
            }
        }
    }
}
