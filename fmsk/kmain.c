#include "commonSettings.h"
#include "vga.h"
#include "idt.h"
#include "herr.h"
#include "memory.h"
#include "../multiboot.h"
#include "pit.h"
#include "ata.h"

unsigned int systemMemoryB = 0;
extern void initGDT();


// The kernel SHOULD get loaded at 0x100000 aka 1MB
void kmain(multiboot_info_t* mbd, uint32_t magic) {
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC) herr("Invalid multiboot magic");
    if(!(mbd->flags >> 6 & 0x1)) herr("Invalid memory map");
    setColorAttribute(0x3B);
    clearScr();
    print(VERSION); printChar('\n');
    print("CMDLine: "); print((const char*)mbd->cmdline); printChar('\n');
    systemMemoryB = mbd->mem_upper * 1024;
    printInt(systemMemoryB / 1024 / 1000); print("MB Ready\n");
    if(mbd->mods_count > 0) herr("Unsupported kernel modules loaded");

    initGDT();
    initPIT(1000);
    initIDT();
    initMemory(systemMemoryB, (*(uint32_t*)(mbd->mods_addr + 4) + 0xFFF) & ~0xFFF);
    initKmalloc(0x1000);
    ataInit();
    enableCursor(9, 11);

    while(1);
}