#include "../lib/acky.h"

void main() {
    char uinput[32];

    while(1) {
        print("Acky> ");
        getInput(uinput);

        if(uinput[0] == '\0') continue;

        const int childPID = spawnProcess(uinput);
        if(childPID == SRET_ERROR) print("[!!]: Not found/Invalid executable\n");
        else while(processRunning(childPID));

        uinput[0] = '\0';
    }
}