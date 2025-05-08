#pragma once
#include "ata.h"
#include "memory.h"
#include "serial.h"
#include <stdint.h>
#include <stdbool.h>

#define LOC_ATA_MASTER 0
#define LOC_ATA_SLAVE 1
#define FS_TYPE_ISO9660 0
#define BLOCK_SIZE 2048

typedef struct {
    char name[32];
    char letter;
    unsigned int lbaStart;
    unsigned int lbaEnd;
    unsigned int loc;
    unsigned int type;
} drive;

typedef struct {
    char name[32];
    uint32_t size;
    uint32_t lbaLoc;
} fileInfo;

extern bool FSsetSTI;

void initFS();
int parsePartitionTable(const uint8_t* buffer, const char letter, const char* name, unsigned int loc);
int addDrive(char letter, const char* name, const unsigned int lbaStart, const unsigned int lbaEnd, const unsigned int loc, unsigned int type);
int iso9660Read(const char *path, int idx, uint8_t *outputBuffer, fileInfo *info);
int fsReadFile(const char *path, uint8_t *buffer, fileInfo *info);
int fsGetFileInfo(const char *path, fileInfo *info);
int iso9660GetFileInfo(const char *path, int idx, fileInfo *info);