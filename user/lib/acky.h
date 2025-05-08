#pragma once
#include <stdint.h>

enum SyscallReturn {
    SRET_SUCCESS,
    SRET_ERROR
};

extern void exit();
extern void print(const char *str);
extern void getInput(char *buf);
extern int spawnProcess(const char *path);
extern int processRunning(const uint32_t childPID);
extern void clearScr();