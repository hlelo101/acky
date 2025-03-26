#pragma once
#include "utils.h"
#include <stdint.h>
#include <stddef.h>

void *memcpy(void *dest, const void *src, int n);
void *memset(void *ptr, int value, size_t num);
void strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);