#include "ps2kbd.h"
#include "io.h"
#include "vga.h"
#include <stdbool.h>

bool leftShift = false;
bool rightShift = false;
bool capsLock = false;

// Kenrel info
bool altPressed = false; // 56, 184
bool leftCtrlPressed = false; // 29, 157
bool kPressed = false; // 37, 165

char scancodeToChar(const int scancode, bool upperCase) {
    const char tableUpper[] = "   1234567890°+ AZERTYUIOP¨£QSDFGHJKLM\% µWXCVBN?./§     ";
    const char tableLower[] = "   &e\"'(-e_ca)=  azertyuiop^$  qsdfghjklmù *wxcvbn,;:!     ";
    if(upperCase) return tableUpper[scancode + 1];
    else return tableLower[scancode + 1];
}

__attribute__((interrupt)) void ps2KBDISR(struct interruptFrame *interruptFrame __attribute__((unused))) {
    SET_DS(0x10);
    SET_ES(0x10);
    
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
        case 56: // Alt pressed
            altPressed = true;
            break;
        case 184: // Alt released
            altPressed = false;
            break;
        case 29: // Left ctrl pressed
            leftCtrlPressed = true;
            break;
        case 157: // left ctrl released
            leftCtrlPressed = false;
            break;
        case 165: // K released
            kPressed = false;
            break;
        case 37: // K pressed
            kPressed = true;
            if(altPressed && leftCtrlPressed && leftShift) break;
            [[fallthrough]];
        default:
            if(scanCode & 0x80) {
                outb(0x20, 0x20);
                return;
            }
            printChar(scancodeToChar(scanCode, capsLock || leftShift || rightShift));
    }

    if(kPressed && altPressed && leftCtrlPressed && leftShift) print(" # WIP; Kernel key # ");
    outb(0x20, 0x20);

    SET_DS(0x0F);
    SET_ES(0x0F);
}