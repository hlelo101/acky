#include <acky.h>

void main() {
    enable13hMode();
    setColor(3);
    putPixel(0, 0);
    setPrimaryCoordinates(0, 1);
    setSecondaryCoordinates(319, 199);
    drawLine();
    exit();
}