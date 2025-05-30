#include "vga.h"
#include "memory.h"
#include "io.h"

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
*/

void ioDelay() {
    for(int i = 0; i < 1000; i++);
}

void writeReg(const int reg, const int index, const int value) {
    if(reg == REG_ATTRIBUTE) {
        inb(REG_ATTRIBUTE_RESET);
        outb(REG_ATTRIBUTE, index);
        outb(REG_ATTRIBUTE, value);
        ioDelay();
        return;
    }
    if (reg == REG_MISC) {
        outb(REG_MISC, value);
        ioDelay();
        return;
    }
    outb(reg, index);
    outb(DATA(reg), value);
    ioDelay();
}

uint8_t readReg(const int reg, const int index) {
    if (reg == REG_ATTRIBUTE) { // Reading that thing is a mess...
        inb(REG_ATTRIBUTE_RESET);
        ioDelay();
        outb(REG_ATTRIBUTE, index);
        ioDelay();
        uint8_t value = inb(DATA(REG_ATTRIBUTE));
        ioDelay();
        inb(REG_ATTRIBUTE_RESET);
        ioDelay();
        return value;
    }
    if (reg == REG_MISC) {
        return inb(0x3CC);
    }
    outb(reg, index);
    ioDelay();
    return inb(DATA(reg));
}

// That gives us a wonderful 320x200 resolution with 256 colors
void init13h() {
    writeReg(REG_CRTC, 0x11, readReg(REG_CRTC, 0x11) & 0x7F);

    writeReg(REG_MISC, 0x00, 0x63); // Set some default values
    // Sequencer
    writeReg(REG_SEQUENCER, 0x00, 0x03);
    writeReg(REG_SEQUENCER, 0x01, 0x01);
    writeReg(REG_SEQUENCER, 0x02, 0x0F);
    writeReg(REG_SEQUENCER, 0x03, 0x00);
    writeReg(REG_SEQUENCER, 0x04, 0x0E);

    // CRTC
    uint8_t crtcRegs[] = {
        0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
        0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
        0xFF
    };
    for(int i = 0; i < (int)sizeof(crtcRegs); i++) {
        writeReg(REG_CRTC, i, crtcRegs[i]);
    }
    // Graphics
    uint8_t graphicsRegs[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0X05, 0x0F,
        0xFF
    };
    for(int i = 0; i < (int)sizeof(graphicsRegs); i++) {
        writeReg(REG_GRAPHICS, i, graphicsRegs[i]);
    }
    // Attribute
    inb(REG_ATTRIBUTE_RESET);
    for(int i = 0; i <= 0x0F; i++) {
        writeReg(REG_ATTRIBUTE, i, i);
    }
    writeReg(REG_ATTRIBUTE, 0x10, 0x41);
    writeReg(REG_ATTRIBUTE, 0x11, 0x00);
    writeReg(REG_ATTRIBUTE, 0x12, 0x0F);
    writeReg(REG_ATTRIBUTE, 0x13, 0x00);
    writeReg(REG_ATTRIBUTE, 0x14, 0x00);

    // Enable video
    writeReg(REG_ATTRIBUTE, 0x20, 0);

    // Default screen
    uint8_t *vgaMem = (uint8_t*)0xA0000;
    for(int i = 0; i < 320 * 200 / 15; i++) {
        for(int i = 0; i <= 15; i++) *vgaMem++ = i;
    }
}

// Wow maths
int abs(int x) {
    return x < 0 ? -x : x;
}

// Bresenham's Line Algorithm
void drawLine(int x0, int y0, int x1, int y1, uint8_t color) {
  int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1; 
  int err = dx + dy, e2;
 
  for(;;) {
    PUT_PIXEL(x0, y0, color);
    if(x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if(e2 >= dy) { err += dy; x0 += sx; }
    if(e2 <= dx) { err += dx; y0 += sy; }
  }
}