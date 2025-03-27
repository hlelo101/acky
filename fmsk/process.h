#include <stdint.h>
#include <stdbool.h>
#include "memory.h"
#include "fs.h"

typedef struct {
    char name[32];
    bool used;
    uint32_t memStart;
    int memSize;
} process;

void spawnProcess(const char *name, const char *path);