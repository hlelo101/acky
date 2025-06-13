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

typedef struct {
    char signature[4]; // 'DREQ'
    uint8_t type; // 0: Pixel; 1: Line; 2: Rectangle
    union {
        pixelInfo pinfo;
        lineInfo linfo;
    } info;
}__attribute__((packed)) drawReq;

typedef struct {
    char signature[4]; // 'CWIN'
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    char name[20];
}__attribute__((packed)) windowDesc;

typedef struct {
    char signature[6]; // 'WINMOV'
    uint8_t x;
    uint8_t y;
}__attribute__((packed)) windowMov;

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
extern int sendMsg(uint32_t receiverPID, procMsg *msg);
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
extern int _createWindow(windowDesc *win);
extern int enableTextMode();
void drawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b);
void putPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
void drawSquare(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t r, uint8_t g, uint8_t b);
void createWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *name);
void *memcpy(void *dest, const void *src, int n);

