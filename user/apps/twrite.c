#include <acky.h>

int cmpstr(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char input[80];
void main() {
    while(1) {
        getInput(input, sizeof(input));
        if(cmpstr(input, "c:exit") == 0) exit();
    }
}