#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "io.h"

#define PRIMARY 0x1F0
#define SECONDARY 0x170
#define ATA_NOT_PRESENT(index, var, name) \
    var = false; \
    print("[ATA]: No " name " drive found\n");

#define ATA_PRESENT(index, var, name) \
    var = false; \
    print("[ATA]: No " name " drive found\n");

void ataWait();
void ataIdentify(bool master);
void ataInit();
void ataRead(bool master, uint16_t *buffer, uint64_t lba, uint16_t sectorCount);