#include "herr.h"
#include "commonSettings.h"
#include "utils.h"

void herr(const char *str) {
    CLI();
    setColorAttribute(0x4C);
    clearScr();

    print("### OS ERROR ###\nSystem version: " VERSION "\n");
    print("Error message: "); print(str); printChar('\n');
    print("\nThis is a fatal error.\nSystem halted.");

    disableCursor();
    while(1) asm volatile("hlt");
}