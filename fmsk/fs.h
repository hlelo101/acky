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

void initFS();
int parsePartitionTable(const uint8_t* buffer, const char letter, const char* name, unsigned int loc);
int addDrive(char letter, const char* name, const unsigned int lbaStart, const unsigned int lbaEnd, const unsigned int loc, unsigned int type);
void fsReadFile(const char *path, uint8_t *buffer);