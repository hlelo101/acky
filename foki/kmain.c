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

bool canPreempt = false;

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

    initSerial();
    serialSendString("Kernel initialization started. Version " VERSION "\n");
    initGDT();
    initPIT(10000);
    initIDT();
    ataInit();
    initFS();
    enableCursor(9, 11);
    initMem();
    
    spawnProcess("Kernel", "A:/ACKYDAT/KPROC.ASA");

    // spawnProcess("Test 2", "A:/ACKYSYS/TEST2.BIN");
    // spawnProcess("Test 3", "A:/ACKYSYS/TEST3.BIN");

    spawnProcess("Init", "A:/ACKYDAT/INIT.ASA");

    serialSendString("[kmain]: Processes spawned\n");

    canPreempt = true;
    while(1);
}