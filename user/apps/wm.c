#include <acky.h>

#define CLEAR() for(int i = 0; i < 320 * 200; i++) { putPixel(i % 320, i / 320); }
#define WM_BG_COLOR 3

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

uint8_t imageData[2500], mouseBG[4 * 4] = {WM_BG_COLOR};
int mouseX = 50, mouseY = 50;

// Some utilities because we still don't have a stdlib
int strcmp(const char *s1, const char *s2) {
    while(*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

void intToStr(int N, char *str) {
    int i = 0;

    int sign = N;

    if (N < 0)
        N = -N;

    while (N > 0) {
        str[i++] = N % 10 + '0';
      	N /= 10;
    } 

    if (sign < 0) {
        str[i++] = '-';
    }

    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

typedef struct {
    char signature[7]; // "MOUMOV"
    int8_t x;
    int8_t y;
} __attribute__((packed)) mouseMovMsg;

// BMP stuff

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

void main() {
    getScreenOwnership(); // Used to indicate to the kernel where to redirect mouse events, etc
    enable13hMode();

    setColor(WM_BG_COLOR);
    CLEAR();

    setColor(1);
    if(loadFile("A:/PROGDAT/WM/BACK.BMP", imageData) == SRET_ERROR) return;
    parseBMP(imageData, 60, 150);
    setColor(0);
    if(loadFile("A:/PROGDAT/WM/MOUSE2.BMP", imageData) == SRET_ERROR) return; // A:/PROGDAT/WM/MOUSESMA.BMP
    // drawMouse(mouseX, mouseY);

    procMsg msg;
    while(1) {
        if(popMsg(&msg) == SRET_SUCCESS) {
            mouseMovMsg *mouseMsg = (mouseMovMsg *)msg.msg;
            if(strcmp(mouseMsg->signature, "MOUMOV") == 0) {
                // Draw the mouse background
                for(int i = 0; i < 4; i++) {
                    for(int j = 0; j < 4; j++) {
                        setColor(mouseBG[i * 4 + j]);
                        putPixel(mouseX + j, mouseY + i);
                    }
                }
                // drawRectangle(mouseX, 4, WM_BG_COLOR);
                mouseX += mouseMsg->x;
                mouseY -= mouseMsg->y;
                if(mouseX < 0) mouseX = 0;
                if(mouseY < 0) mouseY = 0;
                if(mouseX > 320 - 4) mouseX = 320 - 4;
                if(mouseY > 200 - 4) mouseY = 200 - 4;

                // Save the background
                for(int i = 0; i < 4; i++) {
                    for(int j = 0; j < 4; j++) {
                        mouseBG[i * 4 + j] = getPixel(mouseX + j, mouseY + i);
                    }
                }
                // Draw the mouse
                setColor(0);
                parseBMP(imageData, mouseX, mouseY);
            } else {
                serialPrint("Unknown message received\n");
            }
        }
    }
}