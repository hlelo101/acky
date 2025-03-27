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

unsigned int systemMemoryB = 0;
extern void initGDT();

unsigned int getSystemMemory() {
    return systemMemoryB;
}

// The kernel SHOULD get loaded at 0x100000 aka 1MB
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
    initPIT(1000);
    initIDT();
    ataInit();
    initFS();
    enableCursor(9, 11);
    initMem();

    // uint8_t buffer[2048] = {0};
    // fsReadFile("A:/APPS/TEST.BIN", buffer);
    // memcpy((void *)0x300000, buffer, 2048);
    // asm volatile ("jmp 0x300000");
    
    spawnProcess("Test Process", "A:/APPS/TEST.BIN");
    while(1);
}