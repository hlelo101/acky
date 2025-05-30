#include <acky.h>

const uint8_t rawData[] = {
    0b00000001, 0b10000000,
    0b00000011, 0b11000000,
    0b00000111, 0b11100000,
    0b00011111, 0b11111000,
    0b00111111, 0b11111100,
    0b01111111, 0b11111110,
    0b11111111, 0b11111111,
    0b01111111, 0b11111110
};

void drawRaw(const uint8_t *data, int width, int height, int x, int y) {
    // Resolution is 320x200
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            int pixelValue = data[i * width + j];
            for(int bit = 0; bit < 8; bit++) {
                if(pixelValue & (1 << (7 - bit))) {
                    putPixel(x + j * 8 + bit, y + i);
                }
            }
        }
    }
}

void main() {
    enable13hMode();
    setColor(7);
    for(int i = 0; i < 320 * 200; i++) {
        putPixel(i % 320, i / 320);
    }
    setColor(0);
    drawRaw(rawData, 2, 4, 1, 1);
    exit();
}