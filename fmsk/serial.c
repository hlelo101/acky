#include "serial.h"
#include "vga.h"

bool faulty = false;

void initSerial() {
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
    outb(COM1 + 4, 0x1E);
    outb(COM1 + 0, 0xAE);
 
    // Serial faulty?
    if(inb(COM1 + 0) != 0xAE) {
        faulty = true;
        return;
    }

    outb(COM1 + 4, 0x0F);
}

void serialSend(char c) {
    if(faulty) return;
    while((inb(COM1 + 5) & 0x20) == 0);
    outb(COM1, c);
}

void serialSendString(const char* str) {
    for(int i = 0; str[i] != '\0'; i++) {
        serialSend(str[i]);
    }
}

void serialSendInt(int num) {
    char buf[12];
    intToStr(num, buf);
    serialSendString(buf);
}