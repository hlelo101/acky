#include <acky.h>

void intToStr(int num, char *str) {
    int i = 0;
    int isNegative = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0) {
        isNegative = 1;
        num = -num;
    }

    while (num != 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void main() {
    print(
        "Name                            PID                 Memory usage\n"
        "----------------------------------------------------------------\n"
    );
    const int count = getProcCount();
    procInfo info[count];

    for(int i = 1; i < count; i++) {
        const uint32_t pid = getProcPIDFromIdx((uint32_t)i);
        getProcInfo(&info[i], pid);
        print(info[i].name);
        int strSize;
        for(; info[i].name[strSize] != '\0'; strSize++);
        for(int j = 0; j < 32 - strSize; j++) print(" ");
        
        char str[20];
        intToStr(info[i].pid, str);
        print(str);
        strSize = 0;
        for(; str[strSize] != '\0'; strSize++);
        for(int j = 0; j < 20 - strSize; j++) print(" ");
        intToStr(info[i].memUsage, str);
        print(str);
        print("B\n");
    }

    exit();
}