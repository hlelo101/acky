#include <acky.h>

void main() {
    print("IPC testing\n");
    const uint32_t pid = spawnProcess("A:/PROGDAT/IPCCHILD.AEF");

    for(int i = 0; i<=3000; i++);
    procMsg msg;
    print("What do I send to the child process?: ");
    getInput(msg.msg, sizeof(msg.msg));
    sendMsg(pid, &msg);
    exit();
}