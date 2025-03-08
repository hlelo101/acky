#include "ps2kbd.h"
#include "io.h"
#include "vga.h"
#include <stdbool.h>

bool leftShift = false;
bool rightShift = false;
bool capsLock = false;

char scancodeToChar(const int scancode, bool upperCase) {
    const char tableUpper[] = " 1234567890°+   AZERTYUIOP¨£QSDFGHJKLM\% µWXCVBN?./§     ";
    const char tableLower[] = "   &e\"'(-e_ca)=  azertyuiop^$  qsdfghjklmù *wxcvbn,;:!     ";
    if(upperCase) return tableUpper[scancode + 1];
    else return tableLower[scancode + 1];
}

__attribute__((interrupt)) void ps2KBDISR(struct interrupt_frame *interruptFrame __attribute__((unused))) {
    const int scanCode = inb(0x60);
    // You like making key tables don't you?
    //printInt(scanCode); printChar('|');
    
    switch(scanCode) {
        case 28: // Enter
            printChar('\n');
            break;
        case 58: // Caps lock
            capsLock = !capsLock;
            break;
        case 54: // Right shift pressed
            rightShift = true;
            break;
        case 182: // Right shift released
            rightShift = false;
            break;
        case 42: // Left shift pressed
            leftShift = true;
            break;
        case 170: // Left shift released
            leftShift = false;
            break;
        case 14: // Return
            printChar('\b');
            break;
        default:
            if(scanCode & 0x80) {
                outb(0x20, 0x20);
                return;
            }
            printChar(scancodeToChar(scanCode, capsLock || leftShift || rightShift));
    }

    outb(0x20, 0x20);
}