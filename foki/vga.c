#include "vga.h"
#include "memory.h"
#include "io.h"

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