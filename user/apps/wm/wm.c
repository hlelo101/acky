#include <acky.h>
#include "img.h"
#include "window.h"

#define CLEAR() drawSquare(0, 0, 1024, 768, 18, 92, 182);
#define WM_BG_COLOR 3

uint8_t imageData[4096], mouseData[2048];
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
    
    // Font rendering
    loadFile("A:/PROGDAT/WM/FONT.PBF", imageData);
    // renderStr("Hello, World!", imageData, 50, 30, 0, 0, 0);
    // drawChar(imageData, 2, 60, 50, 33, 204, 155);

    // Mouse
    if(loadFile("A:/PROGDAT/WM/MOUSE.BMP", mouseData) == SRET_ERROR) { // A:/PROGDAT/WM/MOUSESMA.BMP
        serialPrint("An error occurred while loading \"A:/PROGDAT/MOUSE.BMP\"");
        while(1);
    }

    // drawWindow(wmCreateWindow("Background", 0, 0, 1024, 786, false, 0));
    drawWindow(wmCreateWindow("Test Window", 500, 500, 400, 200, true, 0));
    drawWindow(wmCreateWindow("UwU", 300, 200,  200, 100, true, 0));

    // Save the initial background
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 12; j++) {
            int index = i * 12 + j;
            mouseBG[index].x = mouseX + j;
            mouseBG[index].y = mouseY + i;
            getPixel(&mouseBG[index]);
        }
    }
    parseBMP(mouseData, mouseX, mouseY, 0, 0, 0);

    spawnProcess("A:/PROGDAT/LAUNCHER.AEF");
    procMsg msg;
    mouseMovMsg *mouseMsg;
    while(1) {
        if(popMsg(&msg) == SRET_SUCCESS) {
            if(memcmp(msg.msg, "MOUMOV", 6) == 0) {
                mouseMsg = (mouseMovMsg *)msg.msg;
                // Draw the mouse background
                for (int i = 0; i < 19; i++) {
                    for (int j = 0; j < 12; j++) {
                        int index = i * 12 + j;
                        putPixel(
                            mouseX + j,
                            mouseY + i,
                            mouseBG[index].r,
                            mouseBG[index].g,
                            mouseBG[index].b
                        );
                    }
                }
                uint8_t r = 0, g = 0;
                if(mouseMsg->status & 0x01) {
                    r = 255;
                    checkPower();
                    int hover = checkWindowHover(mouseX, mouseY);
                    if(hover != -1) {
                        windows[hover].x += mouseMsg->x;
                        windows[hover].y -= mouseMsg->y;

                        drawWindow(hover);
                        if(windows[hover].processPID != 0) {
                            procMsg msg;
                            windowMov *mov = (windowMov *)msg.msg;
                            memcpy(mov->signature, "WINMOV", 6); 
                            mov->x = mouseMsg->x;
                            mov->y = mouseMsg->y;
                            sendMsg(windows[hover].processPID, &msg);
                        }
                    }
                }

                if(mouseMsg->status & 0x02) g = 255;

                mouseX += mouseMsg->x;
                mouseY -= mouseMsg->y;
                if(mouseX < 0) mouseX = 0;
                if(mouseY < 0) mouseY = 0;
                if(mouseX > 1024 - 1) mouseX = 1024 - 1;
                if(mouseY > 768 - 1) mouseY = 768 - 1;

                // Save the background
                for (int i = 0; i < 19; i++) {
                    for (int j = 0; j < 12; j++) {
                        int index = i * 12 + j;
                        mouseBG[index].x = mouseX + j;
                        mouseBG[index].y = mouseY + i;
                        getPixel(&mouseBG[index]);
                    }
                }

                // Draw the mouse
                parseBMP(mouseData, mouseX, mouseY, r, g, 0);
            } else if(memcmp(msg.msg, "DREQ", 4) == 0) {
                drawReq *req = (drawReq *)msg.msg;
                const int idx = getIndexFromPID(msg.fromPID);
                if(idx != -1) {
                    switch(req->type) {
                        case 0: // Pixel
                            if(req->info.pinfo.x > windows[idx].width || req->info.pinfo.y > windows[idx].height) break;
                            putPixel(windows[idx].x + req->info.pinfo.x, windows[idx].y + req->info.pinfo.y, req->info.pinfo.r, 
                                    req->info.pinfo.g, req->info.pinfo.b);
                            break;
                        case 1: // Line
                            if(req->info.linfo.x > windows[idx].width || req->info.linfo.y > windows[idx].height ||
                                req->info.linfo.firstPoint.x > windows[idx].width || req->info.linfo.firstPoint.y > windows[idx].height) break;
                            drawLine(req->info.linfo.firstPoint.x + windows[idx].x, req->info.linfo.y + windows[idx].y,
                                req->info.linfo.x + windows[idx].x, req->info.linfo.y + windows[idx].y,
                                req->info.linfo.firstPoint.r, req->info.linfo.firstPoint.g, req->info.linfo.firstPoint.b);
                        case 2: // Rectangle
                            break;
                    }
                }
            } else if(memcmp(msg.msg, "CWIN", 4) == 0) {
                windowDesc *win = (windowDesc *)msg.msg;
                char str[4];
                drawWindow(wmCreateWindow(win->name, win->x, win->y, win->height, win->width, true, msg.fromPID));
            } else serialPrint("Unknown message received\n");
        }
    }
}
