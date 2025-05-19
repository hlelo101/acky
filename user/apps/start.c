#include <acky.h>

void main() {
    print("Enter the path of the program to run: ");
    char uinput[32];
    getInput(uinput, sizeof(uinput));

    if(spawnProcess(uinput) == 0) print("An error occured\n");

    exit();
}