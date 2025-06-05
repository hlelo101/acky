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
#include "kmain.h"

bool canPreempt = false;
bool systemInitialized = false;
uint16_t VBELoadAddr = 0;

unsigned int systemMemoryB = 0;
extern void initGDT();

unsigned int getSystemMemory() {
    return systemMemoryB;
}

uint32_t loadKFE(const char *path) {
    fileInfo info;
    fsGetFileInfo(path, &info);
    uint8_t buffer[info.size];
    fsReadFile(path, buffer, &info);
    kfeHeader *header = (kfeHeader *)buffer;
    if(memcmp(header->signature, "KFE1", 4) != 0) return 0;
    memcpy((void *)(uintptr_t)header->loadAddr, buffer, info.size);

    return header->loadAddr;
}

uint32_t callKFEFunction(uint8_t index, uint32_t loadAddr) {
    kfeHeader *header = (kfeHeader *)loadAddr;
    if(index >= header->numFuncs) return 0;
    
    func_t f = (func_t)(uintptr_t)header->funcsAddr[index];
    return f();
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
    VBELoadAddr = loadKFE("A:/ACKYDAT/KRME.KFE");
    
    serialSendString("addr = "); serialSendInt(VBEInfo.framebuffer); serialSend('\n');
    serialSendString("pitch = "); serialSendInt(VBEInfo.pitch); serialSend('\n');
    serialSendString("bpp = "); serialSendInt(VBEInfo.bpp); serialSend('\n');

    spawnProcess("A:/ACKYDAT/KPROC.ASA");
    spawnProcess("A:/ACKYDAT/INIT.ASA");
    
    serialSendString("[kmain]: Kernel initialization completed\n");
    
    print("Done\n");
    systemInitialized = true;
    canPreempt = true;
    while(1) asm volatile("hlt");
}
