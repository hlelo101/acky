#pragma once
#include <stdint.h>
#include <acky.h>
#include <stddef.h>

typedef struct {
    char signature[2]; // "BM'
    uint32_t fileSize; // Size in bytes
    uint32_t reserved; // Whatever that's for
    uint32_t dataOffset; // Offset to the pixel data, basically the only thing we care about
}__attribute__((packed)) BMPHeader;

// The standard BITMAPINFOHEADER (40 bytes)
typedef struct {
    uint32_t headerSize;      // Should be 40
    int32_t  width;           // In pixels
    int32_t  height;          // In pixels (positive = bottom-up)
    uint16_t planes;          // Must be 1
    uint16_t bitsPerPixel;    // Should be 1 for monochrome
    uint32_t compression;     // 0 = BI_RGB (no compression)
    uint32_t imageSize;       // May be 0 for BI_RGB
    int32_t  xPelsPerMeter;
    int32_t  yPelsPerMeter;
    uint32_t colorsUsed;      // 0 = default (2 for 1bpp)
    uint32_t colorsImportant; // 0 = all
} __attribute__((packed)) BMPInfoHeader;

typedef struct {
    char signature[3];  // 'PBF'
    uint8_t width;      // Width in bits
    uint8_t height;     // Height in bits
    uint8_t nbEntries;  // Number of characters (entries)
} __attribute__((packed)) PBFHeader;

void parseBMP(const uint8_t* data, int x, int y, uint8_t r, uint8_t g, uint8_t b);
void drawRectangle(int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b);
void drawBorder(int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b);
void drawChar(uint8_t *PBFData, uint8_t index, int x, int y, uint8_t r, uint8_t g, uint8_t b);
