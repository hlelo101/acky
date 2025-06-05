#include "vga.h"
#include "memory.h"
#include "io.h"
#include "gdt.h"
#include "idt.h"
#include "kmain.h"
#include "serial.h"

vbeReturn VBEInfo;
uint32_t ownerPID = 0;
uint8_t currentGMode = VGA_MODE_TEXT;

/*
*   TEXT MODE
*/

char *video_mem = (char *)VIDEO_MEM_ADDR;
int colorAttribute = 0x07;

void intToStr(uint32_t num, char *str) {  // Black magic ChatGPT generated
    uint32_t i = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    while (num != 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    str[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void setColorAttribute(const int attr) {
    colorAttribute =  (int)attr;
}

void printChar(const char c) {
    switch(c) {
        case '\n':
            const int column = ((int)video_mem - VIDEO_MEM_ADDR) % (80*2);
            video_mem += 80*2-column;
            break;
        case '\b':
            if(*(video_mem - 2) == ' ' && (((uint32_t)video_mem / 2) % 80 == 0)) {
                while(*video_mem == ' ') video_mem -= 2;
                video_mem += 2; /// aaagh
            } else video_mem -= 2;
            *video_mem = ' ';
            *(video_mem + 1) = colorAttribute;
            break;
        default:
            *video_mem = c;
            *(video_mem + 1) = colorAttribute;
        
            video_mem += 2;
    }

    if((unsigned int)video_mem >= (VIDEO_MEM_ADDR + 80*25*2)) {
        memcpy((void *)VIDEO_MEM_ADDR, (void *)(VIDEO_MEM_ADDR + 80*2), 80 * 24 * 2);
        video_mem = (char *)(VIDEO_MEM_ADDR + 80*24*2);
        for(char *i = (char *)(VIDEO_MEM_ADDR + 80*24*2); i < (char *)(VIDEO_MEM_ADDR + 80*25*2); i++) {
            if ((i - (char *)VIDEO_MEM_ADDR) % 2) *i = colorAttribute;
            else *i = ' ';
        }        
    }

    updateCursorPos(((const unsigned int)video_mem - VIDEO_MEM_ADDR) / 2);
}

void print(const char *str) {
    while(*str != '\0') {
        printChar(*str);
        str++;
    }
}

void printInt(const uint32_t integer) {
    char str[10] = {0};
    intToStr(integer, str);
    print(str);
}

void clearScr() {
    for(char *i = (char *)VIDEO_MEM_ADDR; (unsigned int)i<=(VIDEO_MEM_ADDR + 80*25*2); i += 2) {
        *i = ' ';
        *(i + 1) = colorAttribute;
    }

    video_mem = (char *)VIDEO_MEM_ADDR;
}

void updateCursorPos(const unsigned int offset) {
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (offset & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((offset >> 8) & 0xFF));
}

void disableCursor() {
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void enableCursor(uint8_t cursor_start, uint8_t cursor_end) {
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

/*
*   GRAPHICS MODE
*   04/06/2025: I gave up on VGA, VBE is much simpler to use, less messy and
*   more capable.
*/

// Wow maths
int abs(int x) {
    return x < 0 ? -x : x;
}

// Bresenham's Line Algorithm
void drawLine(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b) {
  int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1; 
  int err = dx + dy, e2;
 
  for(;;) {
    putPixel(x0, y0, r, g, b);
    if(x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if(e2 >= dy) { err += dy; x0 += sx; }
    if(e2 <= dx) { err += dx; y0 += sy; }
  }
}

void putPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    uint32_t offset = y * VBEInfo.pitch + (x * (VBEInfo.bpp / 8));
    uint8_t *pixel = (uint8_t*)(VBEInfo.framebuffer + offset);
    pixel[2] = b;
    pixel[1] = g;
    pixel[0] = r;
}

void getPixel(int x, int y, pixelInfo *info) {
    uint32_t offset = y * VBEInfo.pitch + (x * (VBEInfo.bpp / 8));
    uint8_t *pixel = (uint8_t*)(VBEInfo.framebuffer + offset);

    info->x = x;
    info->y = y;
    info->r = pixel[0];
    info->g = pixel[1];
    info->b = pixel[2];
}

void enableGraphics() {
    vbeReturn *returnVal = (vbeReturn *)callKFEFunction(0, VBELoadAddr);
    memcpy(&VBEInfo, returnVal, sizeof(vbeReturn));
    initGDT(); // Leaves the system in a half pmode state, so gotta redo these...
    initIDT();

    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            uint8_t r = (x * 255) / SCREEN_WIDTH;
            uint8_t g = (y * 255) / SCREEN_HEIGHT;
            uint8_t b = ((x ^ y) * 255) / (SCREEN_WIDTH > SCREEN_HEIGHT ? SCREEN_WIDTH : SCREEN_HEIGHT);
            putPixel(x, y, r, g, b);
        }
    }
}
