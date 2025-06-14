#pragma once
#include <stdint.h>


typedef struct {
    uint16_t size;
    uint32_t addr;
}__attribute((packed)) idtrDesc;

typedef struct {
    uint16_t offsetLow;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t offsetHigh;
}__attribute__((packed)) gate;

struct interruptFrame {
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t sp;
    uint32_t ss;
};

enum SyscallReturn {
    SRET_SUCCESS,
    SRET_ERROR
};

typedef struct {
    char name[32];
    uint32_t memUsage;
    uint32_t pid;
} userProcInfo;

typedef struct {
    char signature[4]; // CWIN
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    char name[20];
}__attribute__((packed)) windowDesc;

enum syscalls {
    SC_PRINTCHAR,
    SC_GETUINPUT,
    SC_SPAWNPROC,
    SC_EXIT,
    SC_ISPROCRUNNING,
    SC_CLEARSCR,
    SC_GETPORCNUM,
    SC_GETPROCINFO,
    SC_GETPROCPIDFROMIDX,
    SC_SERIALSEND,
    SC_CHPWR,
    SC_MSG,
    SC_CHLAYOUT,
    SC_GRAPHICS,
    SC_LOADFILE,
    SC_GETFILEINFO
};

extern bool runSti;

void initIDT();
void PICMap(int masterOffset, int slaveOffset, uint8_t masterMasks, uint8_t slaveMasks);
void setIDTGate(int index, uint32_t address, uint16_t selector, uint8_t flags);
void setTrampoline(struct interruptFrame *interruptFrame);
__attribute__((naked)) void waitForNextInterrupt();
