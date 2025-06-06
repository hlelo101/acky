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
#define BYTES_TO_BLOCK(b) (((b + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE)

typedef struct {
    uint8_t yearsSince1900;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t gmtOffset;
}__attribute__((packed)) ISO9660DateTime;

typedef struct {
    uint8_t dirRecordLength;
    uint8_t extendedAttributeRecordLength;
    uint32_t LittleEndianLBA;
    uint32_t BigEndianLBA;
    uint32_t LittleEndianDataSize;
    uint32_t BigEndianDataSize;
    ISO9660DateTime dateTime;
    uint8_t flags;
    uint8_t fileUnitSize;
    uint8_t interleavedGapSize;   
    uint16_t littleEndianVolumeSequenceNumber;
    uint16_t bigEndianVolumeSequenceNumber;
    uint8_t fileNameLength;
    char fileName[];
}__attribute__((packed)) ISO9660DirEntry;

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
