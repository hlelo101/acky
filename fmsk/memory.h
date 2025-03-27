#pragma once
#include "utils.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MEM_LIST_SIZE 512 // Fuck yeah, hardcoded values

typedef struct {
    uint32_t startAddr;
    uint32_t size;
} memEntry;

void *memcpy(void *dest, const void *src, int n);
void *memset(void *ptr, int value, size_t num);
void strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
void initMem();
uint32_t allocMem(uint32_t size);