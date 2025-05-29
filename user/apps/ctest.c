#include <acky.h>

uint8_t fileBuffer[2048] = {0};
void main() {
    print("Hai there\nThis is a very testy file \n");

    loadFile("A:/TEST.TXT", fileBuffer);
    print("Contents of \"A:/TEST.TXT\": "); print((const char *)fileBuffer); print("\n");

    exit();
}