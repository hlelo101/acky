#pragma once
#include <stdint.h>

typedef struct {
    char name[32];
    uint32_t memUsage;
    uint32_t pid;
} procInfo;

enum SyscallReturn {
    SRET_SUCCESS,
    SRET_ERROR
};

typedef struct {
    uint32_t fromPID;
    char msg[50];
} procMsg;

typedef struct {
    char name[32];
    uint32_t size;
    uint32_t lbaLoc;
} fileInfo;

extern void exit();
extern void print(const char *str);
extern void getInput(char *buf, const int size);
extern int spawnProcess(const char *path);
extern int processRunning(const uint32_t childPID);
extern void clearScr();
extern int getProcCount();
extern int getProcInfo(procInfo *info, const uint32_t pid);
extern uint32_t getProcPIDFromIdx(const uint32_t idx);
extern int serialPrint(const char *str);
extern void shutdown();
extern void reboot();
extern int sendMsg(const uint32_t receiverPID, procMsg *msg);
extern int popMsg(procMsg *msg);
extern int changeLayout(const int layout); // 0 = FR, 1 = UK
extern void enable13hMode();
extern void setPrimaryCoordinates(int x, int y);
extern void setSecondaryCoordinates(int x, int y);
extern int setColor(uint8_t color);
extern int putPixel(int x, int y);
extern void drawLine();
extern int loadFile(const char *path, uint8_t *buffer);
extern int getFileInfo(const char *path, fileInfo *info);