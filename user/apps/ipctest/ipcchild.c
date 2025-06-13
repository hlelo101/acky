#include <acky.h>

void main() {
    procMsg msg;
    while(1) {
        if(popMsg(&msg) == SRET_SUCCESS) {
            print("Received message: "); print(msg.msg); print("\n");
        }
    }
}
