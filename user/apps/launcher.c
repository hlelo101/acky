#include <acky.h>
#include <stddef.h>

int memcmp(const void *str1, const void *str2, size_t count) {
    register const unsigned char *s1 = (const unsigned char*)str1;
    register const unsigned char *s2 = (const unsigned char*)str2;

    while (count-- > 0) {
        if (*s1++ != *s2++) return s1[-1] < s2[-1] ? -1 : 1;
    }
    return 0;
}

void draw() {
    putPixel(100, 100, 255, 255, 255);
}

void main() {
    createWindow(40, 40, 200, 200, "Test");
   
    draw();
    procMsg msg;
    while(1) {
        if(popMsg(&msg) == SRET_SUCCESS) {
            if(memcmp(msg.msg, "WINMOV", 6) == 0) draw(); 
        }
    }
    exit();
}
