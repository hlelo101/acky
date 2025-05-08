#include <acky.h>
#include <stdbool.h>

char uinput[32];

int cmpstr(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void main() {
    bool firstLoop = false;

    while(1) {
        print("Acky> ");
        getInput(uinput);
        
        if(uinput[0] == '\0') continue;
        
        if(cmpstr(uinput, "cls") == 0) {
            // For some reasons it bugs weirdly on QEMU so this is the solution...
            if(!firstLoop)
                if(cmpstr(uinput, "cls") != 0) goto skipIf;

            clearScr();
            goto end;
        }
        skipIf:

        const int childPID = spawnProcess(uinput);
        if(childPID == SRET_ERROR) print("[!!]: Not found/Invalid executable\n");
        else while(processRunning(childPID));


        end:
        uinput[0] = '\0';
        if(!firstLoop) firstLoop = true;
    }
}