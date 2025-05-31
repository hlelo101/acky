#include <acky.h>

#define CLEAR() for(int i = 0; i < 320 * 200; i++) { putPixel(i % 320, i / 320); }

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

uint8_t imageData[2500];

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

void main() {
    enable13hMode();

    setColor(3);
    CLEAR();

    setColor(1);
    if(loadFile("A:/PROGDAT/WM/BACK.BMP", imageData) == SRET_ERROR) return;
    parseBMP(imageData, 60, 150);
    setColor(0);
    if(loadFile("A:/PROGDAT/WM/MOUSE2.BMP", imageData) == SRET_ERROR) return; // A:/PROGDAT/WM/MOUSESMA.BMP
    parseBMP(imageData, 10, 10);

    exit();
}