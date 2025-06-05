#include <acky.h>
#include "img.h"

#define CLEAR() drawSquare(0, 0, 1024, 768, 18, 92, 182);
#define WM_BG_COLOR 3

uint8_t imageData[9000], mouseData[2048];
pixelInfo mouseBG[12 * 19];

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

    enableGraphicsMode();
    CLEAR();

    drawSquare(0, 0, 1024, 25, 13, 57, 110);

    // Background
    if(loadFile("A:/PROGDAT/WM/BACK.BMP", imageData) == SRET_ERROR) {
        serialPrint("An error occurred while loading \"A:/PROGDAT/WM/BACK.BMP\"");
        while(1);
    }
    parseBMP(imageData, 744, 680, 30, 30, 30);

    // Power
    if(loadFile("A:/PROGDAT/WM/POWER.BMP", imageData) == SRET_ERROR) {
        serialPrint("An error occurred while loading \"A:/PROGDAT/WM/POWER.BMP\"");
        while(1);
    }
    drawSquare(0, 0, 22, 25, 189, 2, 2);
    parseBMP(imageData, 3, 3, 255, 255, 255);
    
    // Character tests
    loadFile("A:/PROGDAT/WM/FONT.PBF", imageData);
    const int charIdx = 1;
    drawChar(imageData, 30, 30, 30, 0, 0, 0);
    drawChar(imageData, '!', 38, 30, 0, 0, 0);
    drawChar(imageData, 32, 46, 30, 0, 0, 0);
    drawChar(imageData, 33, 54, 30, 0, 0, 0);
    drawChar(imageData, 34, 62, 30, 0, 0, 0);
    drawChar(imageData, 35, 70, 30, 0, 0, 0);
    drawChar(imageData, 36, 78, 30, 0, 0, 0);

    // Mouse
    if(loadFile("A:/PROGDAT/WM/MOUSE.BMP", mouseData) == SRET_ERROR) { // A:/PROGDAT/WM/MOUSESMA.BMP
        serialPrint("An error occurred while loading \"A:/PROGDAT/MOUSE.BMP\"");
        while(1);
    }
    // Save the initial background
    for(int i = 0; i < 19; i++) {
        for(int j = 0; j < 12; j++) {
            mouseBG[i * 19 + j].x = mouseX + j;
            mouseBG[i * 19 + j].y = mouseY + i;
            getPixel(&mouseBG[i * 19 + j]);
        }
    }
    parseBMP(mouseData, mouseX, mouseY, 0, 0, 0);

    procMsg msg;
    mouseMovMsg *mouseMsg;
    while(1) {
        if(popMsg(&msg) == SRET_SUCCESS) {
            mouseMsg = (mouseMovMsg *)msg.msg;
            if(strcmp(mouseMsg->signature, "MOUMOV") == 0) {
                // Draw the mouse background
                for(int i = 0; i < 19; i++) {
                    for(int j = 0; j < 12; j++) {
                        putPixel(mouseX + j, mouseY + i, mouseBG[i * 19 + j].r, mouseBG[i * 19 + j].g, mouseBG[i * 19 + j].b);
                    }
                }
                mouseX += mouseMsg->x;
                mouseY -= mouseMsg->y;
                if(mouseX < 0) mouseX = 0;
                if(mouseY < 0) mouseY = 0;
                if(mouseX > 1024 - 1) mouseX = 1024 - 1;
                if(mouseY > 768 - 1) mouseY = 768 - 1;

                // Save the background
                for(int i = 0; i < 19; i++) {
                    for(int j = 0; j < 12; j++) {
                        mouseBG[i * 19 + j].x = mouseX + j;
                        mouseBG[i * 19 + j].y = mouseY + i;
                        getPixel(&mouseBG[i * 19 + j]);
                    }
                }

                uint8_t r = 0, g = 0;
                if(mouseMsg->status & 0x01) {
                    r = 255;
                    checkPower();
                }

                if(mouseMsg->status & 0x02) g = 255; 
                
                // Draw the mouse
                parseBMP(mouseData, mouseX, mouseY, r, g, 0);
            } else serialPrint("Unknown message received\n");
        }
    }
}
