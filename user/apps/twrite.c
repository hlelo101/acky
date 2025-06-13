#include <acky.h>

int cmpstr(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int sizeOf(const char *str) {
    int size = 0;
    while(*str++) size++;
    return size;
}

void strcpy(char *dest, const char *src) {
    while(*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = 0;
}

char input[80];
void main() {
    while(1) {
        getInput(input, sizeof(input));
        if(input[0] == 'c' && input[1] == ':') {
            const int inputSize = sizeOf(input);
            char command[inputSize - 1];
            strcpy(command, (input + 2));
            
            if(cmpstr(command, "exit") == 0) exit();
            else if(cmpstr(command, "cls") == 0) clearScr();
            else if(cmpstr(command, "help") == 0) {
                print(
                    "### Terminal Write - Help menu ###\n"
                    "c:help - Display the help menu\n"
                    "c:exit - Exit TWrite\n"
                    "c:cls  - Clear the screen\n"
                );
            } else {
                print("Error: Unknown command \"");
                print(command); print("\"\n");
            }
        }
    }
}
