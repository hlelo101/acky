#include "ps2kbd.h"

bool leftShift = false;
bool rightShift = false;
bool capsLock = false;

// Kernel key
bool altPressed = false; // 56, 184
bool leftCtrlPressed = false; // 29, 157
bool kPressed = false; // 37, 165

char *processBufferLoc = 0;
int processBufferPos = 0;
int processBufferSize = 0;
bool processWaitingForInput = false;
int inputWaintingPID = 0;
bool layout = true; // false = FR, true = UK

char scancodeToChar(const int scancode, bool upperCase) {
    const char tableUpperFR[] = "   1234567890°+ AZERTYUIOP¨£QSDFGHJKLM\% µWXCVBN?./§     ";
    const char tableLowerFR[] = "   &e\"'(-e_ca)=  azertyuiop^$  qsdfghjklmù *wxcvbn,;:!     ";
    const char tableLowerUK[] = "   1234567890-=  qwertyuiop[]  asdfghjkl;'` #zxcvbnm,./    ";
    const char tableUpperUK[] = "   !\"£\%^&*()_+  QWERTYUIOP{}  ASDFGHJKL:@  ~ZXCVBNM<>?    ";
    if(upperCase) return layout ? tableUpperUK[scancode + 1] : tableUpperFR[scancode + 1];
    else return layout ? tableLowerUK[scancode + 1] : tableLowerFR[scancode + 1];
}

__attribute__((interrupt)) void ps2KBDISR(struct interruptFrame *interruptFrame __attribute__((unused))) {
    SET_DS(0x10);
    SET_ES(0x10);
    
    const int scanCode = inb(0x60);
    // You like making key tables don't you?
    // printInt(scanCode); printChar('|');
    
    switch(scanCode) {
        case 28: // Enter
            if(processWaitingForInput) {
                processWaitingForInput = false;
                processes[inputWaintingPID].waiting = false;
                processBufferLoc[processBufferPos] = '\0'; // Null terminate the string
                processBufferPos = 0;
                processBufferSize = 0;

                printChar('\n');
            }
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
            if((processBufferPos > 0) && processWaitingForInput) {
                processBufferLoc[--processBufferPos] = '\0';
                printChar('\b');
            }
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
                SET_DS(0x0F);
                SET_ES(0x0F);
                outb(0x20, 0x20);
                return;
            }
            char c = scancodeToChar(scanCode, capsLock || leftShift || rightShift);
            if(processWaitingForInput && processBufferPos < processBufferSize - 1) {
                processBufferLoc[processBufferPos++] = c;
                printChar(c);
            }
    }

    if(kPressed && altPressed && leftCtrlPressed && leftShift) print(" # WIP; Kernel key # ");
    outb(0x20, 0x20);

    SET_DS(0x0F);
    SET_ES(0x0F);
}