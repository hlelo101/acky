// Acky startup stub
#include <stdint.h>

#ifndef AEF_NAME
    #error "AEF_NAME undefined. Please define it with the -DAEF_NAME option."
#endif

typedef struct {
    char name[32];
    uint32_t entry;
    char magic[3];
}__attribute__((packed)) aefHeader;

void _start();

__attribute__((section(".text.header")))
const aefHeader header = {
    .name = AEF_NAME,
    .entry = (uint32_t)&_start,
    .magic = {'A', 'E', 'F'}
};

extern void main(void);

__attribute__((section(".text._start")))
void _start() {
    main();

    while(1);
}