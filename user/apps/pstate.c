#include <acky.h>

void main() {
    char input[2] = {0};
    do {
        print("Choose an option:\n1. Shutdown\n2. Reboot\n3. Never mind\n> ");
        getInput(input, sizeof(input));
        switch(input[0]) {
            case '1':
                print("Shutting down...\n");
                shutdown();
                while(1);
                break;
            case '2':
                print("Rebooting...\n");
                reboot();
                break;
            case '3':
                print("Exiting...\n");
                break;
            default:
                print("Invalid option.\n");
        }
    } while(input[0] != '1' && input[0] != '2' && input[0] != '3');

    exit();
}