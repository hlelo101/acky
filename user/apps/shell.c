#include <acky.h>
#include <stdbool.h>

#define INPUT_BUFFER_SIZE 32

char uinput[INPUT_BUFFER_SIZE];
char username[10];

int cmpstr(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void main() {
    do {
        print("Select the keyboard layout (1: FR, AZERTY; 2: UK, QWERTY): ");
        getInput(uinput, 2);

        if(uinput[0] == '1') changeLayout(0);
        else if(uinput[0] == '2') changeLayout(1);
        else print("[!!]: Invalid layout selection\n");
    } while(uinput[0] != '1' && uinput[0] != '2');
    uinput[0] = '\0'; uinput[1] = '\0';

    print("Enter your username: "); getInput(username, 10);
    if(username[0] == '\0') {
        char defaultUsername[] = "user";
        for(int i = 0; i < 5; i++) username[i] = defaultUsername[i];
    }

    while(1) {
        print(username); print("@Acky> ");
        getInput(uinput, INPUT_BUFFER_SIZE);
        
        if(uinput[0] == '\0') continue;
        
        if(cmpstr(uinput, "cls") == 0) {
            clearScr();
            goto end;
        }
        if(cmpstr(uinput, "help") == 0) {
            print("Good luck\n:3\n");
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
    }
}