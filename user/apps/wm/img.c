#include "img.h"

void parseBMP(const uint8_t* data, int x, int y) {
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
                putPixel(x + col, y + row);
            }
        }
    }
}

void drawRectangle(int x, int y, int width, int height, uint8_t color) {
    setColor(color);
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            putPixel(x + j, y + i);
        }
    }
}

void drawBorder(int x, int y, int width, int height, uint8_t color) {
    setColor(color);
    
    setPrimaryCoordinates(x, y);
    setSecondaryCoordinates(x + width, y);
    drawLine();
    setSecondaryCoordinates(x, y + height);
    drawLine();

    setPrimaryCoordinates(x + width, y + height);
    setSecondaryCoordinates(x, y + height);
    drawLine();
    setSecondaryCoordinates(x + width, y);
    drawLine();
}