#include "img.h"

int memcmp(const void *str1, const void *str2, size_t count) {
    register const unsigned char *s1 = (const unsigned char*)str1;
    register const unsigned char *s2 = (const unsigned char*)str2;

    while (count-- > 0) {
        if (*s1++ != *s2++) return s1[-1] < s2[-1] ? -1 : 1;
    }
    return 0;
}

void parseBMP(const uint8_t* data, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    BMPHeader *header = (BMPHeader *)data;
    if(header->signature[0] != 'B' || header->signature[1] != 'M') return;

    BMPInfoHeader *infoHeader = (BMPInfoHeader*)(data + sizeof(BMPHeader));
    if(infoHeader->bitsPerPixel != 1) return;

    int32_t height = infoHeader->height > 0 ? infoHeader->height : -infoHeader->height;
    int32_t width = infoHeader->width;
    int64_t rowBytes = ((width + 31) / 32) * 4;
    const uint8_t* pixels = data + header->dataOffset;
    
    for (int32_t row = 0; row < height; row++) {
        const uint8_t* rowPtr = pixels + ((height - 1 - row) * rowBytes);

        for (int32_t col = 0; col < width; col++) {
            int32_t byteIndex = col >> 3;
            int32_t bitIndex  = 7 - (col & 0x7);

            if(rowPtr[byteIndex] & (1 << bitIndex)) {
                putPixel(x + col, y + row, r, g, b);
            }
        }
    }
}

void drawRectangle(int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b) {
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            putPixel(x + j, y + i, r, g, b);
        }
    }
}

void drawBorder(int x, int y, int width, int height, uint8_t r, uint8_t g, uint8_t b) {
    drawLine(x, y, x + width, y, r, g, b);
    drawLine(x, y, x, y + height, r, g, b);

    drawLine(x + width, y + height, x, y + height, r, g, b);
    drawLine(x + width, y + height, x + width, y, r, g, b);
}

void drawChar(uint8_t *PBFData, uint8_t index, int x, int y, uint8_t r, uint8_t g, uint8_t b) {

    PBFHeader *header = (PBFHeader *)PBFData;
    if(memcmp(header->signature, "PBF", 3) != 0) return; 
    if(index >= header->nbEntries) return; 

    uint8_t bytesPerRow = (header->width + 7) / 8;
    uint16_t charSize = bytesPerRow * header->height;

    uint8_t *glyphData = (PBFData + sizeof(PBFHeader)) + (index * charSize);
    for(uint8_t row = 0; row < header->height; row++) {
        for(uint8_t col = 0; col < header->width; col++) {
            uint8_t byte = glyphData[row * bytesPerRow + (col / 8)];
            uint8_t bit = 7 - (col % 8);
            if(byte & (1 << bit)) {
                putPixel(x + col, y + row, r, g, b);
            }
        }
    }
}

void renderStr(const char *str, uint8_t *fontData, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    int offset = x;
    do {
        drawChar(fontData, *str, offset, y, r, g, b);
        offset += 8;
    } while(*str++);
}
