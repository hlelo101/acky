#include <acky.h>

char uinput[32] = {0};

void main() {
    print("Enter the path of the program to run: ");
    getInput(uinput, sizeof(uinput));

    if(spawnProcess(uinput) == 1) print("An error occurred\n");

    exit();
}
