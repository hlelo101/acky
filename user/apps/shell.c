#include <acky.h>
#include <stdbool.h>

#define INPUT_BUFFER_SIZE 32

char uinput[INPUT_BUFFER_SIZE];

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
        getInput(uinput, INPUT_BUFFER_SIZE);
        
        if(uinput[0] == '\0') continue;
        
        if(cmpstr(uinput, "cls") == 0) {
            // For some reasons it bugs weirdly on QEMU so this is the solution...
            if(!firstLoop)
                if(cmpstr(uinput, "cls") != 0) goto skipIf;

            clearScr();
            goto end;
        }
        skipIf:

        bool reRan = false;
        reRun:
        const int childPID = spawnProcess(uinput);
        if(childPID == SRET_ERROR) {
            if(!reRan) {
                reRan = true;

                int uinputSize = 0;
                for(; uinput[uinputSize] != '\0'; uinputSize++);
            
                if(uinputSize + 11 >= INPUT_BUFFER_SIZE) {
                    print("[!!]: Could not append the PROGDAT path, buffer too small\n");
                    continue;
                }

                const char progdatPath[] = "A:/PROGDAT/";
                for(int i = 0; i <= uinputSize; i++) uinput[i + 11] = uinput[i];
                for(int i = 0; i < 11; i++) uinput[i] = progdatPath[i];

                goto reRun;
            }
            print("[!!]: Not found/Invalid executable\n");
        }
        else while(processRunning(childPID));


        end:
        uinput[0] = '\0';
        if(!firstLoop) firstLoop = true;
    }
}