#include <acky.h>

void main() {
    print("Enter the path of the program to run: ");
    char uinput[32];
    getInput(uinput, sizeof(uinput));

    if(spawnProcess(uinput) == 1) print("An error occurred\n");

    exit();
}
