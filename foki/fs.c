#include "fs.h"
#include "vga.h"
#include "herr.h"

drive drives[26];
int driveCount = 0;

bool FSsetSTI = true;

void commonRead(const unsigned int loc, const unsigned int lba, const unsigned int count, uint8_t* buffer) {
    switch(loc) {
        case LOC_ATA_MASTER:
            ataRead(true, (uint16_t *)buffer, lba * 4, count * 4);
            break;
        default:
            return;
    }
}

int addDrive(char letter, const char* name, const unsigned int lbaStart, const unsigned int lbaEnd, const unsigned int loc, unsigned int type) {
    drives[driveCount].lbaStart = lbaStart;
    drives[driveCount].lbaEnd = lbaEnd;
    drives[driveCount].letter = letter;
    drives[driveCount].loc = (unsigned int)loc;
    strcpy(drives[driveCount].name, name);
    drives[driveCount].type = type;
    driveCount++;

    return driveCount - 1;
}

bool isISO9660(const unsigned int loc) {
    uint8_t buffer[2048];
    commonRead(loc, 16, 1, buffer);
    const char id[] = "CD001";
    for(int i = 1; i <= 5; i++) if(buffer[i] != id[i - 1]) return false;
    return true;
}

int parsePartitionTable(const uint8_t* buffer, const char letter, const char* name, unsigned int loc) {
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
        return addDrive(letter, name, lbaStart, partitionsize, loc, type);
    }

    return 0;
}

int getDriveIndexFromLetter(const char c) {
    for(int i = 0; i < driveCount; i++) {
        if(drives[i].letter == c) return i;
    }
    return -1;
}

// A path should look something like this: "A:/folder/file.ext"
int iso9660Read(const char *path, int idx, uint8_t *outputBuffer, fileInfo *info) {
    iso9660GetFileInfo(path, idx, info);

    if(info->size == 0) return -1; // Not found
    // Read the file
    commonRead(drives[idx].loc, info->lbaLoc, info->size / 2048, outputBuffer);

    return 0;
}

// A path should look something like this: "A:/folder/file.ext"
int iso9660GetFileInfo(const char *path, int idx, fileInfo *info) {
    // Process the path
    int pathSize = 0;
    while(path[pathSize] != '\0') pathSize++;
    char processedPath[pathSize + 3];
    strcpy(processedPath, path);
    processedPath[pathSize] = ';';
    processedPath[pathSize + 1] = '1';
    processedPath[pathSize + 2] = '\0';

    // Read the root folder location
    uint8_t buffer[2048];
    commonRead(drives[idx].loc, 16, 1, buffer);
    // Root directory entry offset is 156
    uint32_t lastFolderLoc = buffer[156 + 2] | (buffer[156 + 3] << 8) | (buffer[156 + 4] << 16) | (buffer[156 + 5] << 24);
    uint8_t fileSize = 0;
    bool lastDirIsFile = false;
    // Go through each directories
    int lastSlashOffset = 3;

    // Get the target folder name
    char folderName[32];
    while(processedPath[lastSlashOffset] != '\0') {
        int j = lastSlashOffset;
        while(processedPath[j] != '/' && processedPath[j] != '\0') {
            folderName[j - lastSlashOffset] = processedPath[j];
            j++;
        }
        folderName[j - lastSlashOffset] = '\0';
        lastSlashOffset = j + 1;

        // At this point, we have the folder name, try to find it in the last dir by iterating through each entry
        commonRead(drives[idx].loc, lastFolderLoc, 1, buffer);
        int entryOffset = 0;
        while(buffer[entryOffset] != 0) {
            // Check the name
            uint8_t dirEntryLength = buffer[entryOffset];
            uint8_t nameLength = buffer[entryOffset + 32];
            fileSize = buffer[entryOffset + 26];
            char dirName[32] = {0};
            memcpy(dirName, buffer + entryOffset + 33, nameLength);

            // If the object is a file, it ends with ;1
            if(dirName[nameLength - 2] == ';' && dirName[nameLength - 1] == '1') lastDirIsFile = true;
            if(strcmp(dirName, folderName) == 0) {
                // Found the folder, get the location
                lastFolderLoc = buffer[entryOffset + 2] | (buffer[entryOffset + 3] << 8) | (buffer[entryOffset + 4] << 16) | (buffer[entryOffset + 5] << 24);
                break;
            }

            entryOffset += dirEntryLength;
        }
    }
    if(!lastDirIsFile) return -1; // Not found

    for(int i = 0; i<32; i++) info->name[i] = folderName[i];
    info->size = ((fileSize == 0) ? 2048 : fileSize);
    info->lbaLoc = lastFolderLoc;

    return 0;
}

int fsReadFile(const char *path, uint8_t *buffer, fileInfo *info) {
    if(getDriveIndexFromLetter(path[0]) < 0) return -1;

    switch(drives[getDriveIndexFromLetter(path[0])].type) {
        case FS_TYPE_ISO9660:
            return iso9660Read(path, getDriveIndexFromLetter(path[0]), buffer, info);
        default:
            return -1;
    }
}

int fsGetFileInfo(const char *path, fileInfo *info) {
    if(getDriveIndexFromLetter(path[0]) < 0) return -1;

    switch(drives[getDriveIndexFromLetter(path[0])].type) {
        case FS_TYPE_ISO9660:
            return iso9660GetFileInfo(path, getDriveIndexFromLetter(path[0]), info);
        default:
            return -1;
    }
}

void initFS() {
    const char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int count = 0;
    if(ataDiskPresent(true)) {
        serialSendString("[FS]: Adding master drive\n");
        uint8_t buffer[512];
        ataRead(true, (uint16_t *)buffer, 0, 1);
        parsePartitionTable(buffer, letters[count++], "Primary Master", LOC_ATA_MASTER);
    }
    if(ataDiskPresent(false)) {
        serialSendString("[FS]: Adding slave drive\n");
        uint8_t buffer[512];
        ataRead(false, (uint16_t *)buffer, 0, 1);
        parsePartitionTable(buffer, letters[count++], "Primary Slave", LOC_ATA_SLAVE);
    }

    if(count == 0) herr("No compatible drives found");
}