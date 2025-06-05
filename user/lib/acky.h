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

typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint16_t x;
    uint16_t y;
} __attribute__((packed)) pixelInfo;

typedef struct {
    pixelInfo firstPoint;
    uint16_t x;
    uint16_t y;
}__attribute__((packed)) lineInfo;

extern int exit();
extern int print(const char *str);
extern int getInput(char *buf, const int size);
extern int spawnProcess(const char *path);
extern int processRunning(const uint32_t childPID);
extern int clearScr();
extern int getProcCount();
extern int getProcInfo(procInfo *info, const uint32_t pid);
extern uint32_t getProcPIDFromIdx(const uint32_t idx);
extern int serialPrint(const char *str);
extern int shutdown();
extern int reboot();
extern int sendMsg(const uint32_t receiverPID, procMsg *msg);
extern int popMsg(procMsg *msg);
extern int changeLayout(const int layout); // 0 = FR, 1 = UK
extern int loadFile(const char *path, uint8_t *buffer);
extern int getFileInfo(const char *path, fileInfo *info);
extern int getScreenOwnership();
extern int getPixel(pixelInfo *info);
extern int enableGraphicsMode();
extern int _putPixel(pixelInfo *info);
extern int _drawLine(lineInfo *info);
extern int _drawSquare(lineInfo *info);
void drawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b);
void putPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
void drawSquare(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t r, uint8_t g, uint8_t b);
