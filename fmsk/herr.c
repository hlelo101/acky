#include "herr.h"
#include "commonSettings.h"

void herr(const char *str) {
    setColorAttribute(0x4C);
    clearScr();

    print("### OS ERROR ###\nSystem version: " VERSION "\n");
    print("Error message: "); print(str); printChar('\n');
    print("\nThis is a fatal error.\nSystem halted.");

    disableCursor();
    while(1) asm volatile("hlt");
}