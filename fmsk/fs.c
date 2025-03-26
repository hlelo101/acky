#include "fs.h"

drive drives[26];
int driveCount = 0;

void addDrive(char letter, const char* name, const unsigned int lbaStart, const unsigned int lbaEnd, const unsigned int loc, unsigned int type) {
    drives[driveCount].lbaStart = lbaStart;
    drives[driveCount].lbaEnd = lbaEnd;
    drives[driveCount].letter = letter;
    drives[driveCount].loc = (unsigned int)loc;
    strcpy(drives[driveCount].name, name);
    drives[driveCount].type = type;
    driveCount++;
}

bool isISO9660(const unsigned int loc) {
    uint8_t buffer[512];
    ataRead(loc == LOC_ATA_MASTER ? true : false, (uint16_t *)buffer, (loc == LOC_ATA_MASTER || loc == LOC_ATA_SLAVE) ? 64 : 16, 1);
    const char id[] = "CD001";
    for(int i = 1; i <= 5; i++) if(buffer[i] != id[i - 1]) return false;
    return true;
}

void parsePartitionTable(const uint8_t* buffer, const char letter, const char* name, unsigned int loc) {
    for(int i = 0; i < 4; i++) {
        const int offset = 446 + 16 * i;

        uint32_t lbaStart = *(uint32_t *)&buffer[offset + 0x08];
        uint32_t partitionsize = *(uint32_t *)&buffer[offset + 0x0C];

        if(lbaStart == 0) continue;

        int type;
        if(isISO9660(loc)) {
            serialSendString("[FS]: Found ISO9660 filesystem\n");
            type = FS_TYPE_ISO9660;
        }
        else type = 512;
        addDrive(letter, name, lbaStart, partitionsize, LOC_ATA_MASTER, type);
    }
}

void iso9660ReadPVD(const int diskLoc) {
    uint8_t buffer[2048];
    ataRead(diskLoc == LOC_ATA_MASTER ? true : false, (uint16_t *)buffer, 64, 1); // 64 because an ATA drive uses 512 byte sectors
    serialSendString("[FS]: ID: "); 
    for(int i = 1; i<=5; i++) {
        serialSend(buffer[i]);
    }
    serialSendString("\n");

    serialSendString("[FS]: Volume Identifier: ");
    for(int i = 0; i < 32; i++) {
        serialSend(buffer[0x20 + i]);
    }
    serialSendString("\n");
}

void initFS() {
    const char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int count = 0;
    if(ataDiskPresent(true)) {
        serialSendString("[FS]: Adding master drive\n");
        uint8_t buffer[512];
        ataRead(true, (uint16_t *)buffer, 0, 1);
        parsePartitionTable(buffer, letters[count++], "Primary Master", LOC_ATA_MASTER);

        iso9660ReadPVD(LOC_ATA_MASTER);
    }
    if(ataDiskPresent(false)) {
        serialSendString("[FS]: Adding slave drive\n");
        uint8_t buffer[512];
        ataRead(false, (uint16_t *)buffer, 0, 1);
        parsePartitionTable(buffer, letters[count++], "Primary Slave", LOC_ATA_SLAVE);
    }
}