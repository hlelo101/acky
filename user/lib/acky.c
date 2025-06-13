#include <acky.h>

void *memcpy(void *dest, const void *src, int n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    while (n--) {
        *d++ = *s++;
    }

    return dest;
}

void drawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b) {
    pixelInfo first;
    first.r = r;
    first.g = g;
    first.b = b;
    first.x = x1;
    first.y = y1;
    lineInfo line;
    line.firstPoint = first;
    line.x = x2;
    line.y = y2;

    _drawLine(&line);
}

void putPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    pixelInfo pixel;
    pixel.x = x;
    pixel.y = y;
    pixel.r = r;
    pixel.g = g;
    pixel.b = b;
    _putPixel(&pixel);
}

void drawSquare(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t r, uint8_t g, uint8_t b) {
    lineInfo line;
    pixelInfo first;
    first.x = x;
    first.y = y;
    first.r = r;
    first.g = g;
    first.b = b;
    line.firstPoint = first;
    line.x = width;
    line.y = height;

    _drawSquare(&line);
}

void createWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *name) {
    windowDesc win;
    win.x = x;
    win.y = y;
    win.width = width;
    win.height = height;
    memcpy(&win.name, name, 20);

    _createWindow(&win);
}
