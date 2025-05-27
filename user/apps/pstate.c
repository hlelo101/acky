#include <acky.h>

void main() {
    print("Shutting down...\n");
    shutdown();
    print("It's now safe to turn off your computer.\n"); // In case something happens and Acky can't shutdown
    while(1);
}