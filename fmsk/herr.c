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
    generalRegs regs;
    asm volatile (
        "mov %%eax, %[eax]\n" "mov %%ebx, %[ebx]\n" "mov %%ecx, %[ecx]\n"
        "mov %%edx, %[edx]\n"
        : [eax] "=m" (regs.eax), [ebx] "=m" (regs.ebx), [ecx] "=m" (regs.ecx),
          [edx] "=m" (regs.edx)
        :
        : "%eax", "%ebx", "%ecx", "%edx"
    );
    print("  EAX: "); printInt(regs.eax); printChar('\n');
    print("  EBX: "); printInt(regs.ebx); printChar('\n');
    print("  ECX: "); printInt(regs.ecx); printChar('\n');
    print("  EDX: "); printInt(regs.edx); printChar('\n');

    disableCursor();
    while(1) asm volatile("hlt");
}