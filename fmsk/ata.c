#include "ata.h"
#include "utils.h"
#include "pit.h"
#include "serial.h"
#include "vga.h"

bool primaryMasterPresent = false;
bool primarySlavePresent = false;
bool primaryMasterSupportsLBA48Mode = false;
bool primarySlaveSupportsLBA48Mode = false;

void ataWait() {
    for(int i = 0; i < 16; i++) inb(PRIMARY + 7);
    while((inb(PRIMARY + 7) & 0x80));
}

void ataWaitReady() {
    while(!(inb(PRIMARY + 7) & 0x8) || (inb(PRIMARY + 7) & 0x1));
}

void ataIdentify(bool master) {
    CLI();
    // Select drive
    outb(PRIMARY + 6, master ? 0xA0 : 0xB0);
    // Set sectorcount, LBAlo, LBAmid, and LBAhi to 0
    outb(PRIMARY + 2, 0);
    outb(PRIMARY + 3, 0);
    outb(PRIMARY + 4, 0);
    outb(PRIMARY + 5, 0);
    // Send IDENTIFY command
    outb(PRIMARY + 7, 0xEC);
    if(inb(PRIMARY + 7) == 0) {
        if(master) serialSendString("[ATA]: Master drive not found\n");
        else serialSendString("[ATA]: Slave drive not found\n");
        if(master) primaryMasterPresent = false;
        else primarySlavePresent = false;
        STI();
        return;
    }
    ataWait();
    ataWaitReady();

    // Read data
    uint16_t data[256];
    for(int i = 0; i < 256; i++) data[i] = inw(PRIMARY);
    if(data[0] == 0) {
        if(master) serialSendString("[ATA]: Master drive not found\n");
        else serialSendString("[ATA]: Slave drive not found\n");
        if(master) primaryMasterPresent = false;
        else primarySlavePresent = false;
        STI();
        return;
    }
    // Extract some useful info from the data
    if(data[83] & 0x400) {
        if(master) primaryMasterSupportsLBA48Mode = true;
        else primarySlaveSupportsLBA48Mode = true;
    }

    if(master) {
        primaryMasterPresent = true;
        serialSendString("[ATA]: Master drive found\n");
    } else {
        primarySlavePresent = true;
        serialSendString("[ATA]: Slave drive found\n");
    }
    STI();
}

void ataRead(bool master, uint16_t *buffer, uint64_t lba, uint16_t sectorCount) {
    if(master && !primaryMasterPresent) return;
    else if(!master && !primarySlavePresent) return;

    CLI();
    // Send the data and commands
    outb(PRIMARY + 6, master ? 0x40 : 0x50);

    outb(PRIMARY + 2, sectorCount >> 8);
    outb(PRIMARY + 3, (lba >> 24) & 0xFF);
    outb(PRIMARY + 4, (lba >> 32) & 0xFF);
    outb(PRIMARY + 5, (lba >> 40) & 0xFF);

    outb(PRIMARY + 2, sectorCount & 0xFF);
    outb(PRIMARY + 3, lba & 0xFF);
    outb(PRIMARY + 4, (lba >> 8) & 0xFF);
    outb(PRIMARY + 5, (lba >> 16) & 0xFF);

    outb(PRIMARY + 7, 0x24);
    ataWait();

    // Read data
    for(int i = 0; i < sectorCount; i++) {
        for(int j = 0; j < 256; j++) buffer[i * 256 + j] = inw(PRIMARY);
        ataWait();
    }
    STI();
}

int boolToInt(bool b) {
    return b ? 1 : 0;
}

void ataInit() {
    ataIdentify(true);
    ataIdentify(false);

    print("[ATA]: Found " ); printInt(boolToInt(primaryMasterPresent) + boolToInt(primarySlavePresent));
    print((boolToInt(primaryMasterPresent) + boolToInt(primarySlavePresent)) == 1 ? " drive\n" : " drives\n");
}

bool ataDiskPresent(bool master) {
    return master ? primaryMasterPresent : primarySlavePresent;
}