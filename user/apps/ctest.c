void print(const char *str) {
    asm volatile (
        "pusha\n"
        "movl %0, %%ecx\n"
        "movl $0, %%eax\n"
        ".loop:\n"
        "movb (%%ecx), %%bl\n"
        "int $0x40\n"
        "inc %%ecx\n"
        "cmpb $0, (%%ecx)\n"
        "jne .loop\n"
        "popa\n"
        :
        : "r" (str)
        : "eax", "ebx", "ecx", "memory"
    );
}

void exit() {
    asm volatile("movl $3, %eax\nint $0x40\n");
}

void main() {
    print("Hai there\nDon't you like it when things just work?\n");
    exit();
}