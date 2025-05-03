#include "herr.h"
#include "commonSettings.h"
#include "utils.h"
#include "process.h"

void herr(const char *str) {
    CLI();
    setColorAttribute(0x4C);
    clearScr();

    print("### OS ERROR ###\nSystem version: " VERSION "\n");
    print("Error message: "); print(str); printChar('\n');
    print("\nThis is a fatal error.\nSystem halted.\n\n");
    // Show register contents
    print("Registers:\n");
    const uint32_t *esp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    print("  EAX: "); printInt(esp[0]); printChar('\n');
    print("  EDX: "); printInt(esp[1]); printChar('\n');
    print("  ECX: "); printInt(esp[2]); printChar('\n');
    print("  EBX: "); printInt(esp[3]); printChar('\n');

    disableCursor();
    while(1) asm volatile("hlt");
}

void herr2(const char *str, const int eip) {
    CLI();

    setColorAttribute(0x4C);
    clearScr();

    print("### OS ERROR ###\nSystem version: " VERSION "\n");
    print("Error message: "); print(str); printChar('\n');
    print("\nThis is a fatal error.\nSystem halted.\n\n");
    // Show register contents
    print("Registers:\n");
    const uint32_t *esp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    print("  EAX: "); printInt(esp[0]); printChar('\n');
    print("  EDX: "); printInt(esp[1]); printChar('\n');
    print("  ECX: "); printInt(esp[2]); printChar('\n');
    print("  EBX: "); printInt(esp[3]); printChar('\n');

    print("EIP: "); printInt(eip); printChar('\n');

    disableCursor();

    asm volatile("xchgw %bx, %bx"); // Magic break
    while(1) asm volatile("hlt");
}