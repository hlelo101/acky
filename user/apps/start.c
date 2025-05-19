#include <acky.h>

void main() {
    print("Enter the path of the program to run: ");
    char uinput[32];
    getInput(uinput, sizeof(uinput));

    spawnProcess("A:/PROGDAT/TEST2.AEF");
    //if(spawnProcess(uinput) != SRET_SUCCESS) print("An error occured\n");

    exit();
}