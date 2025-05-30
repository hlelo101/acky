#include "herr.h"
#include "commonSettings.h"
#include "utils.h"
#include "process.h"

void herr(const char *str) {
    CLI();
    asm volatile("push %eax\npush %edx\npush %ecx\npush %ebx\n");
    setColorAttribute(HERR_COLOR_ATTRIBUTE);
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

    print("Last process: "); print(processes[schedulerProcessAt].name); printChar('\n');
    asm volatile("pop %ebx\npop %ecx\npop %edx\npop %eax\n");

    serialSendString("[Herr]: System stopped due to a kernel panic.\n");
    disableCursor();
    while(1) asm volatile("hlt");
}

void intHerr(const char *str, struct interruptFrame *interruptFrame) {
    CLI();
    asm volatile("push %eax\npush %edx\npush %ecx\npush %ebx\n");

    setColorAttribute(HERR_COLOR_ATTRIBUTE);
    clearScr();

    serialSendString("[Herr]: System stopped due to a kernel panic.\n[Herr]: Error message: ");
    serialSendString(str); serialSend('\n');
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

    print("EIP: "); printInt(interruptFrame->ip); printChar('\n');
    print("CS: "); printInt(interruptFrame->cs); printChar('\n');
    print("SS: "); printInt(interruptFrame->ss); printChar('\n');
    if(interruptFrame->cs == 0x07) {
        kill(schedulerProcessAt);
        interruptFrame->ss = 0x0F;
        setColorAttribute(DEFAULT_COLOR);
        asm volatile("pop %ebx\npop %ecx\npop %edx\npop %eax\n");
        return;
    }

    print("Last process: "); print(processes[schedulerProcessAt].name); printChar('\n');
    asm volatile("pop %ebx\npop %ecx\npop %edx\npop %eax\n");

    disableCursor();

    asm volatile("xchgw %bx, %bx"); // Magic break
    while(1) asm volatile("hlt");
}