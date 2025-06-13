#include <acky.h>

void main() {
    print("IPC testing\n");
    uint32_t pid = spawnProcess("A:/PROGDAT/IPCCHILD.AEF");

    procMsg msg;
    print("What do I send to the child process?: ");
    getInput(msg.msg, sizeof(msg.msg));
    sendMsg(pid, &msg);
    exit();
}
