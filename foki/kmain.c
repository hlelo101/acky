#include "commonSettings.h"
#include "vga.h"
#include "idt.h"
#include "herr.h"
#include "memory.h"
#include "../multiboot.h"
#include "pit.h"
#include "ata.h"
#include "serial.h"
#include "fs.h"
#include "process.h"
#include "gdt.h"
#include "acpi.h"
#include "ps2mouse.h"

bool canPreempt = false;
bool systemInitialized = false;

unsigned int systemMemoryB = 0;
extern void initGDT();

unsigned int getSystemMemory() {
    return systemMemoryB;
}

void kmain(multiboot_info_t* mbd, uint32_t magic) {
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC) herr("Invalid multiboot magic");
    if(!(mbd->flags >> 6 & 0x1)) herr("Invalid memory map");
    setColorAttribute(DEFAULT_COLOR);
    clearScr();
    print(VERSION); printChar('\n');
    print("CMDLine: "); print((const char*)mbd->cmdline); printChar('\n');
    systemMemoryB = mbd->mem_upper * 1024;
    printInt(systemMemoryB / 1000 / 1000); print("MB Ready\n");
    if(mbd->mods_count > 0) herr("Unsupported kernel modules loaded");

    print("Starting kernel... ");
    initSerial();
    serialSendString("Kernel initialization started. Version " VERSION "\n");
    initGDT();
    enableCursor(9, 11);
    initPIT(1000);
    initIDT();
    initACPI();
    ataInit();
    initFS();
    initMem();
    initPS2Mouse();
    
    spawnProcess("A:/ACKYDAT/KPROC.ASA");
    spawnProcess("A:/ACKYDAT/INIT.ASA");
    
    serialSendString("[kmain]: Kernel initialization completed\n");
    
    print("Done\n");
    systemInitialized = true;
    canPreempt = true;
    while(1) asm volatile("hlt");
}