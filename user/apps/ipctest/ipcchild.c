#include <acky.h>

void main() {
    procMsg msg;
    while(1) {
        int res = popMsg(&msg);
        if(res == SRET_SUCCESS) {
            print("Received message: "); print(msg.msg); print("\n");
        }
    }
}